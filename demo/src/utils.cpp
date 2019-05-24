#include <utils.h>
#include <cstring>
#include <cmath>

bool init(const nr::string name, const nr::ScreenDimension& dim, GLFWwindow*& wnd)
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    wnd = glfwCreateWindow(dim.width, dim.height, name.c_str(), NULL, NULL);
    if (!wnd)
    {
        glfwTerminate();    
        return false;
    }

    glfwMakeContextCurrent(wnd);

    glewExperimental = true;
    auto err = glewInit();
    if (err != GLEW_OK)
    {
        glfwDestroyWindow(wnd);
        glfwTerminate();
        std::cerr << glewGetErrorString(err) << std::endl;
        return false;
    }

    glfwSetKeyCallback(wnd, key_callback);
    glfwSwapInterval(1);

    return true;
}

_nr::Module mkFullModule(const NRuint dim, cl_int* err)
{
    NRchar options_fmt[] = "-cl-std=CL2.0 -Werror -D _DEBUG -D RENDER_DIMENSION=%d";
    NRchar options[sizeof(options_fmt) * 2];
    sprintf(options, options_fmt, dim);

    return _nr::Module(
                {_nr::clcode::base,
                 _nr::clcode::bin_rasterizer,
                 _nr::clcode::fine_rasterizer,
                 _nr::clcode::vertex_shading},
                 options,
                 err);
}

nr::FrameBuffer mkFrameBuffer(const nr::ScreenDimension& dim, cl_int* err)
{
    const NRuint totalScreenSize = dim.width * dim.height;

    nr::FrameBuffer ret;
    ret.color = nr::Buffer(CL_MEM_READ_WRITE, 3 * sizeof(NRubyte) * totalScreenSize, err);  
    if (nr::error::isFailure(*err)) return ret;
    ret.depth = nr::Buffer(CL_MEM_READ_WRITE, sizeof(NRfloat) * totalScreenSize, err);
    return ret;
}

FullPipeline::FullPipeline(_nr::Module module, cl_int* err)
{
    vertexShader = module.makeKernel<_nr::VertexShadingParams>("shade_vertex", err);
    if (*err != CL_SUCCESS) return;
    binRasterizer = module.makeKernel<_nr::BinRasterizerParams>("bin_rasterize", err);
    if (*err != CL_SUCCESS) return;
    fineRasterizer = module.makeKernel<_nr::FineRasterizerParams>("fine_rasterize", err);
}

cl_int FullPipeline::setup(
    const NRuint dim,
    const NRuint triangleCount, NRfloat* vertecies, NRfloat* near, NRfloat* far,
    nr::ScreenDimension screenDim, _nr::BinQueueConfig config,                  
    const NRuint binRasterWorkGroupCount, nr::FrameBuffer frameBuffer)
{
    cl_int ret = CL_SUCCESS;
    
    const NRuint binCountX = ceil(((NRfloat) screenDim.width) / config.binWidth);
    const NRuint binCountY = ceil(((NRfloat) screenDim.height) / config.binHeight);
    const NRuint totalBinCount = binCountX * binCountY;
    const NRuint totalScreenDim = screenDim.width * screenDim.height;
    
    // Vertex Shader
    vertexShader.params.points = nr::Buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, dim * 3 * sizeof(NRfloat) * triangleCount, vertecies, &ret);
    if (nr::error::isFailure(ret)) return ret;

    vertexShader.params.near   = nr::Buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, dim * sizeof(NRfloat), near, &ret);
    if (nr::error::isFailure(ret)) return ret;

    vertexShader.params.far    = nr::Buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, dim * sizeof(NRfloat), far, &ret);
    if (nr::error::isFailure(ret)) return ret;

    vertexShader.params.result = nr::Buffer(CL_MEM_READ_WRITE, dim * 3 * sizeof(NRfloat) * triangleCount, &ret);
    if (nr::error::isFailure(ret)) return ret;

    vertexShader.global = cl::NDRange(triangleCount * 3);
    vertexShader.local  = cl::NDRange(1);


    // Bin rasterizer
    binRasterizer.params.triangleData   = vertexShader.params.result;
    binRasterizer.params.triangleCount  = triangleCount;
    binRasterizer.params.dimension      = screenDim;
    binRasterizer.params.binQueueConfig = config;
    binRasterizer.params.hasOverflow    = nr::Buffer(CL_MEM_WRITE_ONLY, sizeof(cl_bool), &ret);
    if (nr::error::isFailure(ret)) return ret;
    binRasterizer.params.binQueues = nr::Buffer(CL_MEM_READ_WRITE, 3 * dim * binRasterWorkGroupCount * totalBinCount * (config.queueSize + 1), &ret);
    if (nr::error::isFailure(ret)) return ret;

    binRasterizer.global = cl::NDRange(binRasterWorkGroupCount * binCountX, binCountY);
    binRasterizer.local  = cl::NDRange(binCountX, binCountY);

    // Fine rasterizer
    fineRasterizer.params.triangleData   = vertexShader.params.result;
    fineRasterizer.params.workGroupCount = binRasterWorkGroupCount;
    fineRasterizer.params.dim            = screenDim;
    fineRasterizer.params.binQueueConfig = config;
    fineRasterizer.params.binQueues      = binRasterizer.params.binQueues;
    fineRasterizer.params.frameBuffer    = frameBuffer;

    fineRasterizer.global = cl::NDRange(binCountX, binCountY);
    fineRasterizer.local  = cl::NDRange(binCountX / binRasterWorkGroupCount, binCountY);

    return CL_SUCCESS;
}

cl_int FullPipeline::operator()(cl::CommandQueue q)
{
    const NRuint totalScreenDim = fineRasterizer.params.dim.width * fineRasterizer.params.dim.height;
    cl_int cl_err = CL_SUCCESS;
    
    printf("Enqueuing vertex shader\n");
    if ((cl_err = vertexShader(q)) != CL_SUCCESS) return cl_err;
    if ((cl_err = q.finish()) != CL_SUCCESS) return cl_err;

    printf("Enqueuing bin rasterizer\n");   
    if ((cl_err = binRasterizer(q)) != CL_SUCCESS) return cl_err;
    if ((cl_err = q.finish()) != CL_SUCCESS) return cl_err;

    printf("Enqueuing fine rasterizer\n");
    if ((cl_err = fineRasterizer(q)) != CL_SUCCESS) return cl_err;
    return q.finish();

    return CL_SUCCESS;
}

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void reduce4Simplex(const Tetrahedron& tetrahedron, Triangle4d result[4])
{
    result[0].points[0] = tetrahedron.points[0];
    result[0].points[1] = tetrahedron.points[1];
    result[0].points[2] = tetrahedron.points[2];

    result[1].points[0] = tetrahedron.points[0];
    result[1].points[1] = tetrahedron.points[1];
    result[1].points[2] = tetrahedron.points[3];

    result[2].points[0] = tetrahedron.points[0];
    result[2].points[1] = tetrahedron.points[2];
    result[2].points[2] = tetrahedron.points[3];

    result[3].points[0] = tetrahedron.points[1];
    result[3].points[1] = tetrahedron.points[2];
    result[3].points[2] = tetrahedron.points[3];
}

Vector4d::Vector4d(const NRfloat x, const NRfloat y, const NRfloat z, const NRfloat w)
    : x(x), y(y), z(z), w(w)
{
}

Vector4d::Vector4d()
    : x(0), y(0), z(0), w(0)
{
}

NRfloat Vector4d::dot(const Vector4d& other) const
{
    return x * other.x + y * other.y + z * other.z + w * other.w;
}

Vector4d Vector4d::operator-(const Vector4d& other) const
{
    return Vector4d(x - other.x, y - other.y, z - other.z, w - other.w);
}


bool Vector4d::operator==(const Vector4d& other) const
{
    return (x == other.x) && (y == other.y) && (z == other.z) && (w == other.w);
}

std::ostream& operator<<(std::ostream& os, const Vector4d& self)
{
    return os << "Vector4d{" << self.x << ", " << self.y << ", " << self.z << ", " << self.w << "}";
}

std::ostream& operator<<(std::ostream& os, const Triangle4d& self)
{
    return os << "Triangle4d{" << self.points[0] << ", " << self.points[1] << ", " << self.points[2] << "}";
}

std::ostream& operator<<(std::ostream& os, const Tetrahedron& self)
{
    return os << "Tetrahedron{" << self.points[0] << ", " << self.points[1] << ", " << self.points[2] << ", " << self.points[3] << "}";
}

bool isOrthgonal(const Vector4d& p1, const Vector4d& p2, const Vector4d& p3)
{
    auto v1 = p1 - p2;
    auto v2 = p2 - p3;
    return fabs(v1.dot(v2)) <= 10e-4;
}

bool isCubeFace(const Vector4d& p1, const Vector4d& p2, const Vector4d& p3, const Vector4d& p4)
{
    return isOrthgonal(p1, p2, p3) && isOrthgonal(p2, p3, p4) && isOrthgonal(p3, p4, p1) && isOrthgonal(p4, p1, p2);
}

void reduceToFaces(const Vector4d cube[8], Vector4d result[6 * 4])
{    
    auto result_idx = 0;
    for (auto i = 0; i < 8 && result_idx < 24; ++i)
    {
        for (auto j = i + 1; j < 8 && result_idx < 24; ++j)
        {
            for (auto k = j + 1; k < 8 && result_idx < 24; ++k)
            {
                for (auto l = k + 1; l < 8 && result_idx < 24; ++l)
                {
                    if (isCubeFace(cube[i], cube[j], cube[k], cube[l]))
                    {
                        result[result_idx++] = cube[i];
                        result[result_idx++] = cube[j];
                        result[result_idx++] = cube[k];
                        result[result_idx++] = cube[l];

                        std::cout << "Cube face:\n  ";
                        std::cout << cube[i] << "  ";
                        std::cout << cube[j] << "  ";
                        std::cout << cube[k] << "  ";
                        std::cout << cube[l] << '\n';
                    }
                    else
                    {
                        std::cout << "Non cube face:\n  ";
                        std::cout << cube[i] << "  ";
                        std::cout << cube[j] << "  ";
                        std::cout << cube[k] << "  ";
                        std::cout << cube[l] << '\n';
                    }
                }   
            }   
        }   
    }
}

bool isVertexInFace(const Vector4d& initial, const Vector4d faces[4])
{
    for (auto vec = 0; vec < 4; ++vec)
        if (initial == faces[vec])
            return false;
    
    return true;
}

void getNextFace(const Vector4d& initial, const Vector4d faces[24], const NRuint lastFaceIndex, NRuint& result)
{
    for (auto face = lastFaceIndex + 1; face < 6; ++face)
    {
        if (!isVertexInFace(initial, faces + 4 * face))
        {
            result = face;
            return;
        }
    }
}

void tetrahadrlize3Cube(const Vector4d cube[8], Tetrahedron result[6])
{
    Vector4d faces[24];
    reduceToFaces(cube, faces);

    NRuint i0, i1, i2;
    getNextFace(faces[0], faces, 0, i0);
    getNextFace(faces[0], faces, i0, i1);
    getNextFace(faces[0], faces, i1, i2);

    result[0].points[0] = faces[0];
    result[0].points[1] = faces[i0 * 4];
    result[0].points[2] = faces[i0 * 4 + 1];
    result[0].points[3] = faces[i0 * 4 + 2];

    result[1].points[0] = faces[0];
    result[1].points[1] = faces[i0 * 4 + 2];
    result[1].points[2] = faces[i0 * 4 + 3];
    result[1].points[3] = faces[i0 * 4];

    result[2].points[0] = faces[0];
    result[2].points[1] = faces[i1 * 4];
    result[2].points[2] = faces[i1 * 4 + 1];
    result[2].points[3] = faces[i1 * 4 + 2];

    result[3].points[0] = faces[0];
    result[3].points[1] = faces[i1 * 4 + 2];
    result[3].points[2] = faces[i1 * 4 + 3];
    result[3].points[3] = faces[i1 * 4];

    result[4].points[0] = faces[0];
    result[4].points[1] = faces[i2 * 4];
    result[4].points[2] = faces[i2 * 4 + 1];
    result[4].points[3] = faces[i2 * 4 + 2];

    result[5].points[0] = faces[0];
    result[5].points[1] = faces[i2 * 4 + 2];
    result[5].points[2] = faces[i2 * 4 + 3];
    result[5].points[3] = faces[i2 * 4];
}

void generate3cube(const Vector4d cube[16], const NRuint diff, const NRuint offset, Vector4d cube3d[8])
{
    auto cube3_idx = 0;
    for (auto i = offset * diff; i < 16; i += 2 * diff)
    {
        for (auto j = 0; j < diff; ++j)
        {
            cube3d[cube3_idx++] = cube[i + j];
        }
    }
}

void cube4dToSimplices(const Vector4d cube[16], Tetrahedron simplices[6 * 8])
{
    Vector4d cube3d[8];

    auto result_idx = 0;
    for (auto diff = 1; diff <= 8; diff *= 2)
    {
        generate3cube(cube, diff, 0, cube3d);
        tetrahadrlize3Cube(cube3d, simplices + result_idx);
        result_idx += 6;
        generate3cube(cube, diff, 1, cube3d);
        tetrahadrlize3Cube(cube3d, simplices + result_idx);
        result_idx += 6;
    }
}

void reduce4Cube(const Vector4d cube[16], Triangle4d result[6 * 8 * 4])
{
    Tetrahedron simplices[6 * 8];
    cube4dToSimplices(cube, simplices);

    for (auto i = 0; i < 48; ++i)
    {
        reduce4Simplex(simplices[i], result + i * 4);
    }
}
