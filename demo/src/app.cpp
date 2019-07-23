#include <app.h>

#include <base/Device.h>
#include <base/Module.h>
#include <base/Platform.h>

#include <kernels/base.cl.h>
#include <kernels/bin_rasterizer.cl.h>
#include <kernels/fine_rasterizer.cl.h>
#include <kernels/simplex_reducing.cl.h>
#include <kernels/vertex_shading.cl.h>

#include <utils/converters.h>

void errorCallback(int error, const char* description)
{
	std::cerr << "GLFW Error: " << description << '\n';
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE); 
}

bool App::init()
{
	if (!glfwInit()) return false;
	glfwSetErrorCallback(errorCallback);

	return true;
}

void App::deinit()
{
	glfwTerminate();
}

App::App(const nr::string& name, const nr::ScreenDimension& m_renderDimension, const nr_uint renderDimension, const nr_uint simplexCount)
	: m_bitmap(new nr::RawColorRGBA[m_renderDimension.width * m_renderDimension.height]), m_far(new nr_float[renderDimension]), m_name(name), m_near(new nr_float[renderDimension]), m_renderDimension(renderDimension), m_screenDim(m_renderDimension), m_h_simplexes(new nr_float[renderDimension * (renderDimension + 1) * simplexCount]), m_simplexCount(simplexCount)
{
}

void App::run()
{
	cl_status status = CL_SUCCESS;

	if (!initialize()) return;
	
	glViewport(0, 0, m_screenDim.width, m_screenDim.height);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	
	loop(&status);
	if (nr::error::isFailure(status))
	{
		std::cerr << "Error detected at main loop: " << nr::utils::stringFromCLError(status) << '\n';
	}

	destroy();
}

nr_uint App::getSimplexCount()
{
	return m_simplexCount;
}

void App::setNearPlane(const nr_float* near)
{
	for (auto i = 0u; i < m_renderDimension; ++i)
	{
		m_near[i] = near[i];
	}
}

void App::setFarPlane(const nr_float* far)
{
	for (auto i = 0u; i < m_renderDimension; ++i)
	{
		m_far[i] = far[i];
	}
}

void App::clearBuffers(cl_status* err)
{
	*err = m_commandQueue.enqueueBufferFillCommand(m_binRasterizer.binQueues, 0u);
	if (nr::error::isFailure(*err)) return;

	*err = m_commandQueue.enqueueBufferFillCommand(m_fineRasterizer.frameBuffer.color, { 0, 0, 0 });
	if (nr::error::isFailure(*err)) return;

	*err = m_commandQueue.enqueueBufferFillCommand(m_fineRasterizer.frameBuffer.depth, 0.0f);
}

void App::destroy()
{
	glfwDestroyWindow(m_window);
}

void App::draw(cl_status* err)
{
	if (nr::error::isFailure(*err = m_vertexShader.load()))
	{
		std::cerr << "Could not load vertex shader arguments: " << nr::utils::stringFromCLError(*err) << '\n';
		return;
	}
	if (nr::error::isFailure(*err = m_commandQueue.enqueueKernelCommand<1>(m_vertexShader, m_vertexShaderGlobalSize, m_vertexShaderLocalSize)))
	{
		std::cerr << "Could not execute vertex shader: " << nr::utils::stringFromCLError(*err) << '\n';
		return;
	}

	if (nr::error::isFailure(*err = m_simplexReducer.load()))
	{
		std::cerr << "Could not load simplex reducer arguments: " << nr::utils::stringFromCLError(*err) << '\n';
		return;
	}
	if (nr::error::isFailure(*err = m_commandQueue.enqueueKernelCommand<1>(m_simplexReducer, m_simplexReducerGlobalSize, m_simplexReducerLocalSize)))
	{
		std::cerr << "Could not execute simplex reducer: " << nr::utils::stringFromCLError(*err) << '\n';
		return;
	}

	if (nr::error::isFailure(*err = m_binRasterizer.load()))
	{
		std::cerr << "Could not load bin rasterizer arguments" << nr::utils::stringFromCLError(*err) << '\n';
		return;
	}
	if (nr::error::isFailure(*err = m_commandQueue.enqueueKernelCommand<2>(m_binRasterizer, m_binRasterizerGlobalSize, m_binRasterizerLocalSize)))
	{
		std::cerr << "Could not execute bin rasterizer: " << nr::utils::stringFromCLError(*err) << '\n';
		return;
	}

	if (nr::error::isFailure(*err = m_fineRasterizer.load()))
	{
		std::cerr <<  "Could not load fine rasterizer arguments" << nr::utils::stringFromCLError(*err) << '\n';
		return;
	}
	if (nr::error::isFailure(*err = m_commandQueue.enqueueKernelCommand<2>(m_fineRasterizer, m_fineRasterizerGlobalSize, m_fineRasterizerLocalSize)))
	{
		std::cerr << "Could not execute fine rasterizer: " << nr::utils::stringFromCLError(*err) << '\n';
		return;
	}

	*err = m_commandQueue.enqueueBufferReadCommand(m_fineRasterizer.frameBuffer.color, true, m_screenDim.width * m_screenDim.height, m_bitmap.get());
	if (nr::error::isFailure(*err))
	{
		std::cerr << "Could not load nr frame buffer: " << nr::utils::stringFromCLError(*err) << '\n';
		return;
	}

	glDrawPixels(640, 480, GL_RGBA, GL_UNSIGNED_BYTE, m_bitmap.get());
}

bool App::initCL()
{
	cl_status ret = CL_SUCCESS;
	auto pret = &ret;

	auto platforms = nr::Platform::getAvailablePlatforms(pret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not aquire available OpenCL platforms: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	if (!platforms.size())
	{
		std::cerr << "No OpenCL platforms found!: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	auto defaultPlatform = platforms[0];

	auto devices = defaultPlatform.getDevicesByType(CL_DEVICE_TYPE_GPU, pret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not aquire OpenCL devices: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	if (!devices.size())
	{
		std::cerr << "No OpenCL supported GPU's found; falling back to CL_DEVICE_TYPE_ALL" << '\n';

		devices = defaultPlatform.getDevicesByType(CL_DEVICE_TYPE_ALL, pret);
		if (nr::error::isFailure(ret))
		{
			std::cerr << "Could not aquire OpenCL devices: " << nr::utils::stringFromCLError(ret) << '\n';
			return false;
		}
	}

	if (!devices.size())
	{
		std::cerr << "No OpenCL devices found!" << '\n';
		return false;
	}

	m_device = devices[0];

	const cl_context_properties props[3] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>((cl_platform_id)defaultPlatform), 0 };
	m_context = nr::Context(props, CL_DEVICE_TYPE_GPU, pret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not create OpenCL context: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	m_commandQueue = nr::CommandQueue(m_context, m_device, (cl_command_queue_properties)CL_QUEUE_PROFILING_ENABLE, pret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not create OpenCL CommandQueue: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	return true;
}

bool App::initialize()
{
	if (!initGL()) return false;
	if (!initCL()) return false;
	if (!initRenderingPipeline()) return false;
	
	return true;
}

bool App::initGL()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	m_window = glfwCreateWindow(m_screenDim.width, m_screenDim.height, m_name.c_str(), NULL, NULL);
	if (!m_window)
	{
		std::cerr << "Could not create window\n";
		return false;
	}

	glfwMakeContextCurrent(m_window);

	glewExperimental = true;
	auto err = glewInit();
	if (err != GLEW_OK)
	{
		glfwDestroyWindow(m_window);
		std::cerr << "Could not initialize GLEW: " << glewGetErrorString(err) << '\n';
		return false;
	}

	glfwSetKeyCallback(m_window, keyCallback);
	glfwSwapInterval(1);

	return true;
}

bool App::initRenderingPipeline()
{
	cl_status ret = CL_SUCCESS;

	nr::__internal::BinQueueConfig config{48, 48, 120};

	const nr_uint binRasterWorkGroupCount = 1;
	const nr_uint binCountX = ceil(((nr_float)m_screenDim.width) / config.binWidth);
	const nr_uint binCountY = ceil(((nr_float)m_screenDim.height) / config.binHeight);
	const nr_uint totalBinCount = binCountX * binCountY;
	const nr_uint totalScreenDim = m_screenDim.width * m_screenDim.height;
	const nr_uint trianglesPerSimplex = m_renderDimension * (m_renderDimension - 1) * (m_renderDimension - 2) / 6;
	const nr_uint triangleCount = trianglesPerSimplex * m_simplexCount;
	const nr_uint totalFloatCount = (m_renderDimension + 1) * 3 * triangleCount;
	
	nr::Module::Options opts = { nr::Module::RenderDimension(m_renderDimension), nr::Module::DEBUG, nr::Module::CL_VERSION_12 };
	nr::Module::Sources srcs = {
		nr::__internal::clcode::base,
		nr::__internal::clcode::bin_rasterizer,
		nr::__internal::clcode::fine_rasterizer,
		nr::__internal::clcode::simplex_reducing,
		nr::__internal::clcode::vertex_shading
	};
	
	auto fullModule = nr::Module(m_context, srcs, &ret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not initialize pipeline module: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}
	ret = fullModule.build(m_device, opts);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not initialize pipeline module: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	m_binRasterizer = nr::__internal::BinRasterizer(fullModule, &ret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not initialize bin rasterizer kernel: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	m_fineRasterizer = nr::__internal::FineRasterizer(fullModule, &ret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not initialize fine rasterizer kernel: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	m_simplexReducer = nr::__internal::SimplexReducer(fullModule, &ret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not initialize simplex reducer kernel: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	m_vertexShader = nr::__internal::VertexShader(fullModule, &ret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not initialize vertex shader kernel: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	// Vertex Shader
	m_vertexShader.points = nr::Buffer<nr_float>(m_context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, (m_renderDimension + 1) * m_renderDimension * m_simplexCount, m_h_simplexes.get(), &ret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not initialize vertex shader point buffer: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	m_vertexShader.near = nr::Buffer<nr_float>(m_context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, m_renderDimension, m_near.get(), &ret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not initialize vertex shader near buffer: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	m_vertexShader.far = nr::Buffer<nr_float>(m_context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, m_renderDimension, m_far.get(), &ret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not initialize vertex shader far buffer: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	m_vertexShader.result = nr::Buffer<nr_float>(m_context, CL_MEM_READ_WRITE, (m_renderDimension + 1) * m_renderDimension * m_simplexCount, &ret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not initialize vertex shader result buffer: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	m_vertexShaderGlobalSize = { m_renderDimension * m_simplexCount };
	m_vertexShaderLocalSize = { 1 };

	// Simplex Reducer
	m_simplexReducer.simplexes = m_vertexShader.result;
	m_simplexReducer.result = nr::Buffer<nr_float>(m_context, CL_MEM_READ_WRITE, totalFloatCount, &ret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not initialize simplex reducer result buffer: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	m_simplexReducerGlobalSize = { m_simplexCount };
	m_simplexReducerLocalSize = { 1 };

	// Bin rasterizer
	m_binRasterizer.triangleData = m_simplexReducer.result;
	m_binRasterizer.triangleCount = triangleCount;
	m_binRasterizer.dimension = m_screenDim;
	m_binRasterizer.binQueueConfig = config;
	
	m_binRasterizer.hasOverflow = nr::Buffer<nr_bool>(m_context, CL_MEM_WRITE_ONLY, 1, &ret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not initialize bin rasterizer overflow buffer: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	m_binRasterizer.binQueues = nr::Buffer<nr_uint>(m_context, CL_MEM_READ_WRITE, binRasterWorkGroupCount * totalBinCount * (config.queueSize + 1), &ret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not initialize bin rasterizer queues: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	m_binRasterizer.batchIndex = nr::Buffer<nr_uint>(m_context, CL_MEM_READ_WRITE, 1, &ret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not initialize bin rasterizer batch inde  buffer: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	m_binRasterizerGlobalSize = { binRasterWorkGroupCount * binCountX, binCountY };
	m_binRasterizerLocalSize = { binCountX, binCountY };

	// Fine rasterizer
	m_fineRasterizer.triangleData = m_vertexShader.result;
	m_fineRasterizer.workGroupCount = binRasterWorkGroupCount;
	m_fineRasterizer.dim = m_screenDim;
	m_fineRasterizer.binQueueConfig = config;
	m_fineRasterizer.binQueues = m_binRasterizer.binQueues;
	
	m_fineRasterizer.frameBuffer = nr::FrameBuffer();
	m_fineRasterizer.frameBuffer.color = nr::Buffer<nr::RawColorRGBA>(m_context, CL_MEM_READ_WRITE, m_screenDim.width * m_screenDim.height, &ret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not initialize color buffer: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}
	m_fineRasterizer.frameBuffer.depth = nr::Buffer<nr_float>(m_context, CL_MEM_READ_WRITE, m_screenDim.width * m_screenDim.height, &ret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not initialize depth buffer: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	m_fineRasterizerGlobalSize = { binCountX, binCountY };
	m_fineRasterizerLocalSize = { binCountX / binRasterWorkGroupCount, binCountY };

	return true;
}

void App::loadData(cl_status* err)
{
	*err = m_commandQueue.enqueueBufferWriteCommand(m_vertexShader.points, false, m_simplexCount * m_renderDimension * (m_renderDimension + 1), m_h_simplexes.get());
}

void App::loop(cl_status* err)
{
	while (!glfwWindowShouldClose(m_window))
	{
		update();
		loadData(err);
		if (nr::error::isFailure(*err)) return;
		
		clearBuffers(err);
		if (nr::error::isFailure(*err)) return;
		
		draw(err);
		if (nr::error::isFailure(*err)) return;
		
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}
}
