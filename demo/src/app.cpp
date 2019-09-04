#include "app.h"

#include <chrono>
#include <iostream>

void errorCallback(int error, const char* description)
{
	std::cerr << "GLFW Error: " << description << '\n';
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE); 
}

void App::deinit()
{
	glfwTerminate();
}

bool App::init()
{
	if (!glfwInit()) return false;
	glfwSetErrorCallback(errorCallback);

	return true;
}

App::App(const nr::string& name, const nr::ScreenDimension& screenDim, const nr_uint renderDimension)
	: m_bitmap(new nr::RawColorRGBA[screenDim.width * screenDim.height]), m_name(name), m_renderDimension(renderDimension), m_screenDim(screenDim), m_pipeline({ 64, 64, 256 }, 8)
{
}

nr_status App::run()
{
	cl_status status = CL_SUCCESS;

	if (!initialize()) return -1;
	
	glViewport(0, 0, m_screenDim.width, m_screenDim.height);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	
	loop(status);
	if (nr::error::isFailure(status))
	{
		std::cerr << "Error detected at main loop: " << nr::utils::stringFromCLError(status) << '\n';
	}

	destroy();
	return status;
}

void App::destroy()
{
	glfwDestroyWindow(m_window);
}

nr_status App::draw(const nr::VertexBuffer& vb, const nr::Primitive& type, const nr_uint primitiveCount)
{
	nr_status ret = m_pipeline.clear();
	if (nr::error::isFailure(ret)) return ret;

	auto t0 = std::chrono::system_clock::now();

	ret = m_pipeline.render(vb, type, primitiveCount);
	if (nr::error::isFailure(ret)) return ret;

	auto t1 = std::chrono::system_clock::now();

	auto diff = std::chrono::duration<nr_double>(t1 - t0);
	
	ret = m_pipeline.copyFrameBuffer(m_bitmap.get());
	if (nr::error::isFailure(ret)) return ret;

	glDrawPixels(m_screenDim.width, m_screenDim.height, GL_RGBA, GL_UNSIGNED_BYTE, m_bitmap.get());

	glfwSwapBuffers(m_window);

	return ret;
}

bool App::initRenderingPipeline()
{
	cl_status ret = CL_SUCCESS;
	

	auto platforms = nr::Platform::getAvailablePlatforms(ret);
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

	auto devices = defaultPlatform.getDevicesByType(CL_DEVICE_TYPE_GPU, ret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not aquire OpenCL devices: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	if (!devices.size())
	{
		std::cerr << "No OpenCL supported GPU's found; falling back to CL_DEVICE_TYPE_ALL" << '\n';

		devices = defaultPlatform.getDevicesByType(CL_DEVICE_TYPE_ALL, ret);
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

	auto device = devices[0];

	const cl_context_properties props[3] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>((cl_platform_id)defaultPlatform), 0 };
	m_context = nr::Context(props, CL_DEVICE_TYPE_GPU, ret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not create OpenCL context: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	m_commandQueue = nr::CommandQueue(m_context, device, (cl_command_queue_properties)CL_QUEUE_PROFILING_ENABLE, ret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not create OpenCL CommandQueue: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	m_pipeline.init(m_context, device, m_commandQueue, m_renderDimension, ret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not initialize rendering pipeline: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	ret = m_pipeline.viewport(m_screenDim);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not initialize rendering pipeline viewport: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	return true;
}

bool App::initialize()
{
	if (!initGL()) return false;
	if (!initRenderingPipeline()) return false;
	if (nr::error::isFailure(init(m_context, m_pipeline))) return false;
	
	return true;
}

bool App::initGL()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_RESIZABLE, 0);

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

bool App::isKeyPressed(int key)
{
	return glfwGetKey(m_window, key) == GLFW_PRESS;
}

void App::loop(cl_status& err)
{
	while (!glfwWindowShouldClose(m_window))
	{
		err = update(m_commandQueue);
		if (nr::error::isFailure(err)) return;
		
		glfwPollEvents();
	}
}
