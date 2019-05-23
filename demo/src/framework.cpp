#include <framework.h>

bool init(const nr::string name, const nr::ScreenDimension& dim, GLFWerrorfun errorCallback, GLFWkeyfun keyCallback, GLFWwindow*& wnd)
{
    glfwSetErrorCallback(errorCallback);
    if (!glfwInit()) return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
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

    glfwSetKeyCallback(wnd, keyCallback);
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

