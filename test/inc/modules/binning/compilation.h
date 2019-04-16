#pragma once

#include <inc/includes.h>

#include <base/Module.h>
#include <kernels/base.cl.h>
#include <kernels/bin_rasterizer.cl.h>
#include <base/Buffer.h>

using namespace nr;
using namespace nr::__internal;
using namespace testing;

TEST(Binning, Compilation)
{
    cl_int err = CL_SUCCESS; 

    Module production({clcode::base, clcode::bin_rasterizer}, "-cl-std=CL2.0 -Werror -D RENDER_DIMENSION=3", &err);
    ASSERT_EQ(CL_SUCCESS, err);

    Module testing({clcode::base, clcode::bin_rasterizer}, "-cl-std=CL2.0 -Werror -D _DEBUG -D _TEST_BINNING -D RENDER_DIMENSION=5 -D SIMPLEX_TEST_COUNT=3", &err);
    ASSERT_EQ(CL_SUCCESS, err);

    auto log = production.getBuildLog(&err);
    ASSERT_EQ(CL_SUCCESS, err);
    ASSERT_EQ("", log) << "Compiling production code failed:\n" << log;

    log = testing.getBuildLog(&err);
    ASSERT_EQ(CL_SUCCESS, err);
    ASSERT_EQ("", log) << "Compiling testing code failed:\n" << log;
}
