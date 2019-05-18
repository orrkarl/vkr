#include <inc/includes.h>

#include <base/Module.h>

using namespace nr;
using namespace nr::__internal;

void testCompilation(const char* options, string configurationName, std::initializer_list<string> codes)
{
    cl_int err = CL_SUCCESS;

    Module code(codes, options, &err);
    ASSERT_EQ(CL_SUCCESS, err);

    auto log = code.getBuildLog(&err);
    ASSERT_EQ(CL_SUCCESS, err);
    ASSERT_EQ("", log) << "Compiling " << configurationName << " failed:" << ":\n" << log;
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

testing::AssertionResult isSuccess(const nr::Error& err)
{
    if (nr::error::isSuccess(err)) return testing::AssertionSuccess();
    else return testing::AssertionFailure() << nr::utils::stringFromNRError(err) << " (" << err << ')';
}

