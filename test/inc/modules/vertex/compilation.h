#pragma once

#include <inc/includes.h>

#include <kernels/base.cl.h>
#include <kernels/vertex_shading.cl.h>

using namespace nr;
using namespace nr::__internal;
using namespace testing;

TEST(VertexShader, Compilation)
{
    testCompilation(Module::Options{Module::CL_VERSION_12, Module::WARNINGS_ARE_ERRORS, Module::RenderDimension(10)}, "production 10d", {clcode::base, clcode::vertex_shading});
    testCompilation(Module::Options{Module::CL_VERSION_12, Module::WARNINGS_ARE_ERRORS, Module::RenderDimension(4), Module::DEBUG}, "debug/testing 4d", {clcode::base, clcode::vertex_shading});
}

