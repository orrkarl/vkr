#include <inc/includes.h>

#include <base/Module.h>

using namespace nr;
using namespace nr::__internal;

void testCompilation(const nr::__internal::Module::Options options, string configurationName, std::initializer_list<string> codes)
{
    cl_int err = CL_SUCCESS;

    Module code(codes, options, &err);
    ASSERT_EQ(CL_SUCCESS, err);

    auto log = code.getBuildLog(&err);
    ASSERT_TRUE(isSuccess(err));
    ASSERT_EQ("", log) << "Compiling " << configurationName << " failed:" << "\n" << log;
}

NRuint index_from_screen(const ScreenPosition& position, const nr::ScreenDimension& dim)
{
    return position.y * dim.width + position.x;
}

NDCPosition ndcFromScreen(const ScreenPosition screen, const nr::ScreenDimension& screenDim)
{
    NDCPosition ndc;
    ndc.x = ((NRfloat) screen.x) * 2.0 / (screenDim.width - 1) - 1;
    ndc.y = ((NRfloat) screen.y) * 2.0 / (screenDim.height - 1) - 1;
    return ndc;
}

testing::AssertionResult isSuccess(const cl_int& err)
{
    if (err == CL_SUCCESS) return testing::AssertionSuccess();
    else return testing::AssertionFailure() << nr::utils::stringFromCLError(err) << " (" << err << ')';
}

nr::__internal::Module mkStandardModule(const NRuint dim, const initializer_list<string>& codes, cl_int* err)
{
    auto options = _nr::Module::Options
    {
        _nr::Module::CL_VERSION_12, 
        _nr::Module::WARNINGS_ARE_ERRORS, 
        _nr::Module::DEBUG, 
        _nr::Module::RenderDimension(dim)
    };

    return _nr::Module(codes, options, err);
}
