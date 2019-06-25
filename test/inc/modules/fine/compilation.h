#pragma once

#include <inc/includes.h>

#include <kernels/base.cl.h>
#include <kernels/fine_rasterizer.cl.h>

#include "fine_utils.h"

using namespace nr;
using namespace nr::__internal;
using namespace testing;

TEST(Fine, Compilation)
{
    testCompilation(Module::Options{Module::CL_VERSION_12, Module::RenderDimension(10)}, "production 10d", {clcode::base, clcode::fine_rasterizer});
    testCompilation(Module::Options{Module::CL_VERSION_12, Module::RenderDimension(8), Module::DEBUG, TEST_FINE}, "debug/testing 8d", {clcode::base, clcode::fine_rasterizer});
}

