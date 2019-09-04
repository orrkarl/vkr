#include "includes.h"

#include <base/Module.h>
#include <base/Platform.h>

nr::Device defaultDevice = nr::Device();
nr::Context defaultContext = nr::Context();
nr::CommandQueue defaultCommandQueue = nr::CommandQueue();

void testCompilation(const nr::Module::Options options, nr::string configurationName, std::initializer_list<nr::string> codes)
{
    cl_status err = CL_SUCCESS;

	nr::Module code(defaultContext, codes, err);
    ASSERT_SUCCESS(err);

    cl_status buildErr = code.build(defaultDevice, options);

    auto log = code.getBuildLog(defaultDevice, err);
    ASSERT_SUCCESS(err);
	
	ASSERT_SUCCESS(buildErr) << "Compiling " << configurationName << " failed:" << "\n" << log;
}

nr_uint indexFromScreen(const ScreenPosition& position, const nr::ScreenDimension& dim)
{
    return position.y * dim.width + position.x;
}

NDCPosition ndcFromScreen(const ScreenPosition screen, const nr::ScreenDimension& screenDim)
{
    NDCPosition ndc;
    ndc.x = screen.x * 2.0f / screenDim.width - 1;
    ndc.y = screen.y * 2.0f / screenDim.height - 1;
    return ndc;
}

NDCPosition ndcFromPixelMid(const ScreenPosition& screen, const nr::ScreenDimension& dim)
{
	NDCPosition ndc;
	ndc.x = (screen.x + 0.5f) * 2.0f / dim.width - 1;
	ndc.y = (screen.y + 0.5f) * 2.0f / dim.height - 1;
	return ndc;
}

testing::AssertionResult isSuccess(const cl_status& err)
{
    if (err == CL_SUCCESS) return testing::AssertionSuccess();
    else return testing::AssertionFailure() << nr::utils::stringFromCLError(err) << " (" << err << ')';
}

nr::Module::Options mkStandardOptions(const nr_uint dim)
{
    return nr::Module::Options{
                nr::Module::CL_VERSION_12, 
                nr::Module::DEBUG, 
                nr::Module::RenderDimension(dim)
            };
}

cl_status init()
{
    cl_status ret = CL_SUCCESS;

    auto platforms = nr::Platform::getAvailablePlatforms(ret);
    if (nr::error::isFailure(ret)) return ret;

    if (!platforms.size())
    {
        std::cerr << "No OpenCL platforms found!" << std::endl;
        return CL_INVALID_PLATFORM;
    }

    auto defaultPlatform = platforms[0];
    
    auto devices = defaultPlatform.getDevicesByType(CL_DEVICE_TYPE_GPU, ret);
    if (nr::error::isFailure(ret)) return ret;

    if (!devices.size())
    {
        std::cerr << "No OpenCL devices found!" << std::endl;
        return CL_INVALID_DEVICE;
    }

    defaultDevice = devices[0];

    const cl_context_properties props[3] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>((cl_platform_id) defaultPlatform), 0};
    auto context = nr::Context(props, CL_DEVICE_TYPE_GPU, ret);
    if (nr::error::isFailure(ret)) return ret;

    defaultContext = context;

	auto q = nr::CommandQueue(context, devices[0], (cl_command_queue_properties) CL_QUEUE_PROFILING_ENABLE, ret);
	if (nr::error::isFailure(ret)) return ret;

	defaultCommandQueue = q;

    return CL_SUCCESS;
}

void destroy()
{
	defaultCommandQueue.flush();
	defaultCommandQueue.finish();
	defaultCommandQueue.release();

	defaultDevice.release();

	defaultContext.release();
}

