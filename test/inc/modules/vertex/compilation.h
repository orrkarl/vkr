#pragma once

#include <inc/includes.h>

#include <kernels/base.cl.h>
#include <kernels/vertex_shading.cl.h>

using namespace nr;
using namespace nr::__internal;
using namespace testing;

TEST(VertexShader, Compilation)
{
    testCompilation("-cl-std=CL2.0 -Werror -D RENDER_DIMENSION=10", "production 3d", {clcode::base, clcode::vertex_shading});
    testCompilation("-cl-std=CL2.0 -Werror -D _DEBUG -D RENDER_DIMENSION=8", "debug/testing 4d", {clcode::base, clcode::vertex_shading});
}

