#pragma once

#include <inc/includes.h>

#include <base/Module.h>
#include <kernels/base.cl.h>
#include <kernels/bin_rasterizer.cl.h>
#include <base/Buffer.h>

using namespace nr;
using namespace nr::__internal;
using namespace testing;

void testCompilation(const char* options)
{
    cl_int err = CL_SUCCESS;

    Module code({clcode::base, clcode::bin_rasterizer}, options, &err);
    ASSERT_EQ(CL_SUCCESS, err);

    auto log = code.getBuildLog(&err);
    ASSERT_EQ(CL_SUCCESS, err);
    ASSERT_EQ("", log) << "Compiling production code failed:\n" << log;

}

TEST(Binning, Compilation)
{
    testCompilation("-cl-std=CL2.0 -Werror -D RENDER_DIMENSION=3");
    testCompilation("-cl-std=CL2.0 -Werror -D _DEBUG -D _TEST_BINNING -D RENDER_DIMENSION=5 -D SIMPLEX_TEST_COUNT=3");
}
