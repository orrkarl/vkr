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
