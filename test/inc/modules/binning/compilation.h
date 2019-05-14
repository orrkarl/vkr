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
    testCompilation("-cl-std=CL2.0 -Werror -D RENDER_DIMENSION=3", "production 3d", {clcode::base, clcode::bin_rasterizer});
    testCompilation("-cl-std=CL2.0 -Werror -D _DEBUG -D _TEST_BINNING -D RENDER_DIMENSION=5 -D TRIANGLE_TEST_COUNT=3", "debug/testing 5d", {clcode::base, clcode::bin_rasterizer});
}

