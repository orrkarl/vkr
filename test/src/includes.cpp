#include <inc/includes.h>

#include <base/Module.h>
#include <base/Platform.h>

using namespace nr;

void testCompilation(const nr::Module::Options options, string configurationName, std::initializer_list<string> codes)
{
    cl_status err = CL_SUCCESS;

    Module code(codes, &err);
    ASSERT_SUCCESS(err);

    ASSERT_SUCCESS(code.build(options));

    auto log = code.getBuildLog(&err);
    ASSERT_SUCCESS(err);

    ASSERT_EQ("", log) << "Compiling " << configurationName << " failed:" << "\n" << log;
}

nr_uint index_from_screen(const ScreenPosition& position, const nr::ScreenDimension& dim)
{
    return position.y * dim.width + position.x;
}

NDCPosition ndcFromScreen(const ScreenPosition screen, const nr::ScreenDimension& screenDim)
{
    NDCPosition ndc;
    ndc.x = ((nr_float) screen.x) * 2.0 / (screenDim.width - 1) - 1;
    ndc.y = ((nr_float) screen.y) * 2.0 / (screenDim.height - 1) - 1;
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
                Module::CL_VERSION_12, 
                Module::WARNINGS_ARE_ERRORS, 
                Module::DEBUG, 
                Module::RenderDimension(dim)
            };
}

cl_status init()
{
    cl_status ret = CL_SUCCESS;
    auto pret = &ret;


    auto platforms = Platform::getAvailablePlatforms(pret);
    if (error::isFailure(ret)) return ret;

    if (!platforms.size())
    {
        std::cerr << "No OpenCL platforms found!" << std::endl;
        return 10000;
    }

    Platform::makeDefault(platforms.front());
    
    
    auto devices = platforms.front().getDevicesByType(CL_DEVICE_TYPE_GPU, pret);
    if (error::isFailure(ret)) return ret;

    if (!devices.size())
    {
        std::cerr << "No OpenCL devices found!" << std::endl;
        return 10000;
    }

    Device::makeDefault(devices.front());
    

    const cl_context_properties props[3] = { CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>((cl_platform_id) Platform::getDefault()), 0};
    auto context = Context(props, CL_DEVICE_TYPE_GPU, pret);
    if (error::isFailure(ret)) return ret;

    Context::makeDefault(context);

    return CL_SUCCESS;
}
