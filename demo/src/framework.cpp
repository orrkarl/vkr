#include <framework.h>

bool init(const nr::string name, GLFWerrorfun errorCallback, GLFWkeyfun keyCallback, GLFWwindow* wnd)
{
    glfwSetErrorCallback(errorCallback);
    if (!glfwInit()) return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    wnd = glfwCreateWindow(640, 480, name.c_str(), NULL, NULL);
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

nr::FrameBuffer mkFrameBuffer(const NRuint w, const NRuint h, nr::Error* err)
{
    nr::FrameBuffer ret;
    ret.color = nr::Buffer(CL_MEM_READ_WRITE, 3 * sizeof(NRubyte) * w * h, err);  
    if (nr::error::isFailure(*err)) return;
    ret.depth = nr::Buffer(CL_MEM_READ_WRITE, sizeof(NRfloat) * w * h, err);
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

nr::Error setupPipeline(
    FullPipeline pipeline,
    const NRuint dim,
    const NRuint triangleCount,         
    nr::ScreenDimension screenDim, _nr::BinQueueConfig config,                  
    const NRuint binRasterWorkGroupCount, nr::FrameBuffer frameBuffer)
{
    nr::Error ret = nr::Error::NO_ERROR;
    
    // Vertex Shader
    pipeline.vertexShader.params.points = nr::Buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, dim * 3 * sizeof(NRfloat) * triangleCount, &ret);
    if (nr::error::isFailure(ret)) return ret;

    pipeline.vertexShader.params.near = nr::Buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, dim * sizeof(NRfloat), &ret);
    if (nr::error::isFailure(ret)) return ret;

    pipeline.vertexShader.params.far = nr::Buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, dim * sizeof(NRfloat), &ret);
    if (nr::error::isFailure(ret)) return ret;

    pipeline.vertexShader.params.result = nr::Buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, dim * 3 * sizeof(NRfloat) * triangleCount, &ret);
    if (nr::error::isFailure(ret)) return ret;


    // Bin rasterizer
    pipeline.binRasterizer.params.triangleData = pipeline.vertexShader.params.result;
    pipeline.binRasterizer.params.triangleCount = triangleCount;
    pipeline.binRasterizer.params.dimension = screenDim;
    pipeline.binRasterizer.params.binQueueConfig = config;
}

