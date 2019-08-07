#pragma once

#include <inc/includes.h>

#include <base/Module.h>
#include <base/Kernel.h>
#include <rendering/Render.h>
#include <kernels/base.cl.h>

using namespace nr;
using namespace nr::detail;
using namespace testing;

TEST(Base, Compilation)
{
    testCompilation(Module::Options{Module::CL_VERSION_12, Module::_3D}, "Production 3d", {clcode::base});
    testCompilation(Module::Options{Module::CL_VERSION_12, Module::RenderDimension(11), Module::DEBUG}, "Debug 11d", {clcode::base});
}
