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
    testCompilation(Module::Options{Module::CL_VERSION_12, Module::WARNINGS_ARE_ERRORS, Module::_3D}, "production 3d", {clcode::base, clcode::bin_rasterizer});    
    testCompilation(Module::Options{Module::CL_VERSION_12, Module::WARNINGS_ARE_ERRORS, Module::_5D, Moduke::DEBUG, Module::Macro("_TEST_BINNING"), Module::Macro("TRIANGLE_TEST_COUNT", "3")}, "debug/testing 5d", {clcode::base, clcode::bin_rasterizer});
}

