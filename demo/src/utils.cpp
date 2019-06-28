#include <utils.h>

#include <cmath>
#include <cstring>

#include <base/Platform.h>

#include <utils/converters.h>

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

    cl_status ret = CL_SUCCESS;
    auto pret = &ret;

    auto platforms = nr::Platform::getAvailablePlatforms(pret);
    if (nr::error::isFailure(ret))
    {
        std::cerr << "Could not aquire available OpenCL platforms: " << nr::utils::stringFromCLError(ret) << std::endl;
        return false;
    }

    if (!platforms.size())
    {
        std::cerr << "No OpenCL platforms found!: " << nr::utils::stringFromCLError(ret) << std::endl;
        return false;
    }

    nr::Platform::makeDefault(platforms[0]);
    
    auto devices = platforms[0].getDevicesByType(CL_DEVICE_TYPE_GPU, pret);
    if (nr::error::isFailure(ret))
    {
        std::cerr << "Could not aquire OpenCL devices: " << nr::utils::stringFromCLError(ret) << std::endl;
        return false;
    }

    if (!devices.size())
    {
        std::cerr << "No OpenCL supported GPU's found; falling back to CL_DEVICE_TYPE_ALL" << std::endl;

        devices = platforms[0].getDevicesByType(CL_DEVICE_TYPE_ALL, pret);
        if (nr::error::isFailure(err))
        {
            std::cerr << "Could not aquire OpenCL devices: " << nr::utils::stringFromCLError(ret) << std::endl;
            return false;
        }
    }

    if (!devices.size())
    {
        std::cerr << "No OpenCL devices found!" << std::endl;
        return false;
    }

    nr::Device::makeDefault(devices[0]);
    
    const cl_context_properties props[3] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>((cl_platform_id) nr::Platform::getDefault()), 0};
    auto context = nr::Context(props, CL_DEVICE_TYPE_GPU, pret);
    if (nr::error::isFailure(ret))
    {
        std::cerr << "Could not create OpenCL context: " << nr::utils::stringFromCLError(ret) << std::endl;
        return false;
    }

    nr::Context::makeDefault(context);

	auto q = nr::CommandQueue(context, devices[0], (cl_command_queue_properties) CL_QUEUE_PROFILING_ENABLE, pret);
	if (nr::error::isFailure(ret))
    {
        std::cerr << "Could not create OpenCL CommandQueue: " << nr::utils::stringFromCLError(ret) << std::endl;
        return false;       
    }

	nr::CommandQueue::makeDefault(q);

    return true;
}

nr::FrameBuffer mkFrameBuffer(const nr::ScreenDimension& dim, cl_status* err)
{
    const nr_uint totalScreenSize = dim.width * dim.height;

    nr::FrameBuffer ret;
    ret.color = nr::Buffer<nr::RawColorRGB>(CL_MEM_READ_WRITE, totalScreenSize, err);  
    if (nr::error::isFailure(*err)) return ret;
    ret.depth = nr::Buffer<nr_float>(CL_MEM_READ_WRITE, totalScreenSize, err);
    return ret;
}

FullModule::FullModule(cl_status* err)
    : nr::Module(STANDARD_MODULE_KERNELS, err)
{
    auto allCodes = {
        nr::__internal::clcode::base,
        nr::__internal::clcode::bin_rasterizer,
        nr::__internal::clcode::fine_rasterizer,
        nr::__internal::clcode::vertex_shading
    };
}

cl_status FullModule::build(const nr_uint dim)
{
    auto opts = nr::Module::Options {
        nr::Module::CL_VERSION_12, 
        nr::Module::DEBUG, 
        nr::Module::RenderDimension(dim)
    };
    return nr::Module::build(opts);
}

const nr::Module::Sources FullModule::STANDARD_MODULE_KERNELS = {
    nr::__internal::clcode::base,
    nr::__internal::clcode::bin_rasterizer,
    nr::__internal::clcode::fine_rasterizer,
    nr::__internal::clcode::vertex_shading
};

FullPipeline::FullPipeline(FullModule module, cl_status* err)
	: vertexShader(module, err), binRasterizer(module, err), fineRasterizer(module, err)
{
}

cl_status FullPipeline::setup(
    const nr_uint dim,
    const nr_uint triangleCount, nr_float* vertecies, nr_float* near, nr_float* far,
    nr::ScreenDimension screenDim, nr::__internal::BinQueueConfig config,                  
    const nr_uint binRasterWorkGroupCount, nr::FrameBuffer frameBuffer)
{
    cl_status ret = CL_SUCCESS;
    
    const nr_uint binCountX = ceil(((nr_float) screenDim.width) / config.binWidth);
    const nr_uint binCountY = ceil(((nr_float) screenDim.height) / config.binHeight);
    const nr_uint totalBinCount = binCountX * binCountY;
    const nr_uint totalScreenDim = screenDim.width * screenDim.height;
    
    // Vertex Shader
    vertexShader.points = nr::Buffer<nr_float>(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, dim * 3 * triangleCount, vertecies, &ret);
    if (nr::error::isFailure(ret)) return ret;

    vertexShader.near = nr::Buffer<nr_float>(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, dim, near, &ret);
    if (nr::error::isFailure(ret)) return ret;

    vertexShader.far = nr::Buffer<nr_float>(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, dim, far, &ret);
    if (nr::error::isFailure(ret)) return ret;

    vertexShader.result = nr::Buffer<nr_float>(CL_MEM_READ_WRITE, dim * 3 * triangleCount, &ret);
    if (nr::error::isFailure(ret)) return ret;

	vertexShaderGlobalSize = { triangleCount * 3 };
	vertexShaderLocalSize  = { 1 };

    // Bin rasterizer
    binRasterizer.triangleData   = vertexShader.result;
    binRasterizer.triangleCount  = triangleCount;
    binRasterizer.dimension      = screenDim;
    binRasterizer.binQueueConfig = config;
    binRasterizer.hasOverflow    = nr::Buffer<nr_bool>(CL_MEM_WRITE_ONLY, 1, &ret);
    if (nr::error::isFailure(ret)) return ret;
    binRasterizer.binQueues = nr::Buffer<nr_uint>(CL_MEM_READ_WRITE, 3 * dim * binRasterWorkGroupCount * totalBinCount * (config.queueSize + 1), &ret);
    if (nr::error::isFailure(ret)) return ret;
    binRasterizer.batchIndex = nr::Buffer<nr_uint>(CL_MEM_READ_WRITE, 1, &ret);
    if (nr::error::isFailure(ret)) return ret;

	binRasterizerGlobalSize = { binRasterWorkGroupCount * binCountX, binCountY };
	binRasterizerLocalSize  = { binCountX, binCountY };

    // Fine rasterizer
    fineRasterizer.triangleData   = vertexShader.result;
    fineRasterizer.workGroupCount = binRasterWorkGroupCount;
    fineRasterizer.dim            = screenDim;
    fineRasterizer.binQueueConfig = config;
    fineRasterizer.binQueues      = binRasterizer.binQueues;
    fineRasterizer.frameBuffer    = frameBuffer;

	fineRasterizerGlobalSize = { binCountX, binCountY };
	fineRasterizerLocalSize  = { binCountX / binRasterWorkGroupCount, binCountY };

    return CL_SUCCESS;
}

cl_status FullPipeline::operator()(nr::CommandQueue q)
{
    const nr_uint totalScreenDim = fineRasterizer.dim.width * fineRasterizer.dim.height;
    cl_status cl_err = CL_SUCCESS;
    
    printf("Enqueuing vertex shader\n");
	if ((cl_err = vertexShader.load()) != CL_SUCCESS) return cl_err;
	if ((cl_err = q.enqueueKernelCommand<1>(vertexShader, vertexShaderGlobalSize, vertexShaderLocalSize)) != CL_SUCCESS) return cl_err;
    if ((cl_err = q.await()) != CL_SUCCESS) return cl_err;
    printf("Vertecies transformed!\n");

    printf("Enqueuing bin rasterizer\n");   
	if ((cl_err = binRasterizer.load()) != CL_SUCCESS) return cl_err;
	if ((cl_err = q.enqueueKernelCommand<2>(binRasterizer, binRasterizerGlobalSize, binRasterizerLocalSize)) != CL_SUCCESS) return cl_err;
	if ((cl_err = q.await()) != CL_SUCCESS) return cl_err;
    printf("Bins filled!\n");

    printf("Enqueuing fine rasterizer\n");
	if ((cl_err = fineRasterizer.load()) != CL_SUCCESS) return cl_err;
	if ((cl_err = q.enqueueKernelCommand<2>(fineRasterizer, fineRasterizerGlobalSize, fineRasterizerLocalSize)) != CL_SUCCESS) return cl_err;
	if ((cl_err = q.await()) != CL_SUCCESS) return cl_err;
    printf("Framebuffer filled - pipeline completed!\n");
    
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

Vector4d::Vector4d(const nr_float x, const nr_float y, const nr_float z, const nr_float w)
    : x(x), y(y), z(z), w(w)
{
}

Vector4d::Vector4d()
    : x(0), y(0), z(0), w(0)
{
}

nr_float Vector4d::dot(const Vector4d& other) const
{
    return x * other.x + y * other.y + z * other.z + w * other.w;
}

Vector4d Vector4d::operator-(const Vector4d& other) const
{
    return Vector4d(x - other.x, y - other.y, z - other.z, w - other.w);
}

nr_float Vector4d::distanceSquared(const Vector4d& other) const
{
    auto diff = *this - other;
    return diff.dot(diff);
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

bool isCubeFace(const Vector4d& p0, const Vector4d& p1, const Vector4d& p2, const Vector4d& p3)
{
    auto d01 = p0.distanceSquared(p1);
    auto d02 = p0.distanceSquared(p2);
    auto d03 = p0.distanceSquared(p3);
    auto d12 = p1.distanceSquared(p2);
    auto d13 = p1.distanceSquared(p3);
    auto d23 = p2.distanceSquared(p3);

    std::vector<nr_float> diffs{ d01, d02, d03, d12, d13, d23 };
    auto min = *std::min_element(diffs.cbegin(), diffs.cend());
    auto max = *std::max_element(diffs.cbegin(), diffs.cend());

    if (fabs(min - 0.5 * max) > 10e-4) return false;

    auto countMin = 0, countMax = 0;
    for (auto i = 0; i < 6; ++i)
    {
        if (diffs[i] - min <= 10e-4) countMin++;
        else if (max - diffs[i] <= 10e-4) countMax++;
        else return false;
    }   

    return countMax == 2 && countMin == 4; 
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
            return true;
    
    return false;
}

void getNextFace(const Vector4d& initial, const Vector4d faces[24], const nr_uint lastFaceIndex, nr_uint& result)
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

    nr_uint i0, i1, i2;
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

void generate3cube(const Vector4d cube[16], const nr_uint diff, const nr_uint offset, Vector4d cube3d[8])
{
    auto cube3_idx = 0;
    for (nr_uint i = offset * diff; i < 16; i += 2 * diff)
    {
        for (nr_uint j = 0; j < diff; ++j)
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
        // printf("Working on cube %d\n", result_idx / 6);
        generate3cube(cube, diff, 0, cube3d);
        tetrahadrlize3Cube(cube3d, simplices + result_idx);
        result_idx += 6;
        // printf("Working on cube %d\n", result_idx / 6);
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
