#pragma once

#include <kernels/vertex_reducer.cl.h>

#include "../../includes.h"


using namespace nr;
using namespace testing;

TEST(VertexShader, Compilation)
{
    testCompilation(Module::Options{Module::CL_VERSION_12, Module::RenderDimension(10)}, "production 10d", {clcode::base, clcode::vertex_reduce });
    testCompilation(Module::Options{Module::CL_VERSION_12, Module::RenderDimension(4), Module::DEBUG}, "debug/testing 4d", {clcode::base, clcode::vertex_reduce });
}

