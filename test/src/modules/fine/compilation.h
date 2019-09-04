#pragma once

#include <kernels/base.cl.h>
#include <kernels/fine_rasterizer.cl.h>

#include "fine_utils.h"

using namespace nr;
using namespace nr::detail;
using namespace testing;

TEST(Fine, Compilation)
{
    testCompilation(Module::Options{ Module::CL_VERSION_12 }, "production", {clcode::base, clcode::fine_rasterizer});
    testCompilation(Module::Options{ Module::CL_VERSION_12, Module::DEBUG, TEST_FINE }, "debug/testing", {clcode::base, clcode::fine_rasterizer});
}

