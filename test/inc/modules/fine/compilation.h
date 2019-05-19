#pragma once

#include <inc/includes.h>

#include <kernels/base.cl.h>
#include <kernels/fine_rasterizer.cl.h>

using namespace nr;
using namespace nr::__internal;
using namespace testing;

TEST(Fine, Compilation)
{
    testCompilation("-cl-std=CL2.0 -Werror -D RENDER_DIMENSION=10", "production 10d", {clcode::base, clcode::fine_rasterizer});
    testCompilation("-cl-std=CL2.0 -Werror -D _DEBUG -D _TEST_FINE -D RENDER_DIMENSION=8", "debug/testing 8d", {clcode::base, clcode::fine_rasterizer});
}

