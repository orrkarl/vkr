#pragma once

#include <inc/includes.h>

#include <base/Module.h>
#include <base/Kernel.h>
#include <rendering/Render.h>
#include <kernels/base.cl.h>

using namespace nr;
using namespace nr::__internal;
using namespace testing;

TEST(Base, Compilation)
{
    testCompilation("-cl-std=CL2.0 -Werror -D RENDER_DIMENSION=3", "Production 3d", {clcode::base});
    testCompilation("-cl-std=CL2.0 -Werror -D _DEBUG -D RENDER_DIMENSION=11", "Debug 11d", {clcode::base});
}
