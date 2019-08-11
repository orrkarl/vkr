#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <general/predefs.h>

#include <base/CommandQueue.h>
#include <base/Context.h>
#include <base/Device.h>
#include <base/Platform.h>

#include <pipeline/Pipeline.h>
#include <pipeline/VertexBuffer.h>

#include <rendering/Render.h>

#include <utils/converters.h>

constexpr const nr_uint dim = 3;

nr::Simplex<dim> h_triangle
{
	nr::Vertex<dim>{ -5, -2.5,   3, 1 },
	nr::Vertex<dim>{  0,	5, 2.5, 1 },
	nr::Vertex<dim>{  5, -2.5,   2, 1 }
};

nr_float h_near[3]
{
    -3, -3, 0.5
};

nr_float h_far[3]
{
    3, 3, 10
};

bool initCL(nr::Context& context, nr::Device& device, nr::CommandQueue& queue)
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

	device = devices[0];

	const cl_context_properties props[3] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>((cl_platform_id)defaultPlatform), 0 };
	context = nr::Context(props, CL_DEVICE_TYPE_GPU, pret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not create OpenCL context: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	queue = nr::CommandQueue(context, device, (cl_command_queue_properties)CL_QUEUE_PROFILING_ENABLE, pret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not create OpenCL CommandQueue: " << nr::utils::stringFromCLError(ret) << '\n';
		return false;
	}

	return true;
}

void errorCallback(int error, const char* description)
{
	std::cerr << "GLFW Error: " << description << '\n';
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

bool initGL(GLFWwindow*& wnd, const nr::ScreenDimension& screenDim, const nr::string& name)
{
	if (!glfwInit()) return false;
	glfwSetErrorCallback(errorCallback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_RESIZABLE, 0);

	wnd = glfwCreateWindow(screenDim.width, screenDim.height, name.c_str(), NULL, NULL);
	if (!wnd)
	{
		std::cerr << "Could not create window\n";
		return false;
	}

	glfwMakeContextCurrent(wnd);

	glewExperimental = true;
	auto err = glewInit();
	if (err != GLEW_OK)
	{
		glfwDestroyWindow(wnd);
		std::cerr << "Could not initialize GLEW: " << glewGetErrorString(err) << '\n';
		return false;
	}

	glfwSetKeyCallback(wnd, keyCallback);
	glfwSwapInterval(1);

	return true;
}

int main(const int argc, const char* argv[])
{
	cl_status ret = CL_SUCCESS;
	auto pret = &ret;

	constexpr nr::ScreenDimension screenDim = { 640, 480 };

	nr::CommandQueue q;
	nr::Context ctx;
	nr::Device dev;
	GLFWwindow* wnd;

	std::unique_ptr<nr::RawColorRGBA[]> bitmap(new nr::RawColorRGBA[screenDim.getTotalSize()]);

	if (!initCL(ctx, dev, q)) return EXIT_FAILURE;
	if (!initGL(wnd, screenDim, "Triangle3d")) return EXIT_FAILURE;

	auto vb = nr::VertexBuffer(ctx, dim, 1, &h_triangle, pret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not create vertex buffer!\n";
		return ret;
	}

	auto p = nr::Pipeline(ctx, dev, q, dim, { 64, 64, 255 }, 1, pret);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Coult not create pipeline!\n";
		return ret;
	}
	ret = p.viewport(screenDim);
	if (nr::error::isFailure(ret))
	{
		std::cerr << "Could not set viewport!\n";
		return ret;
	}
	p.setClearColor({ 0, 0, 0, 0 });
	p.setClearDepth(1.0f);

	while (!glfwWindowShouldClose(wnd))
	{
		ret = p.clear();
		if (nr::error::isFailure(ret))
		{
			std::cerr << "Could clear framebuffer!\n";
			return ret;
		}

		ret = p.render(vb, nr::Primitive::SIMPLEX, 1);
		if (nr::error::isFailure(ret))
		{
			std::cerr << "Could not render! " << nr::utils::stringFromCLError(ret) << std::endl;
			return ret;
		}

		ret = p.copyFrameBuffer(bitmap.get());
		if (nr::error::isFailure(ret))
		{
			std::cerr << "Could not copy framebuffer!\n";
			return ret;
		}

		glDrawPixels(screenDim.width, screenDim.height, GL_RGBA, GL_UNSIGNED_BYTE, bitmap.get());

		glfwSwapBuffers(wnd);
		glfwPollEvents();
	}

	return ret;
}
