#pragma once

#include <kernels/vertex_post_processor.cl.h>

#include "../../../includes.h"


using namespace nr;
using namespace nr::detail;
using namespace testing;

TEST(VertexPostProcessing, Compilation)
{
	testCompilation(Module::Options{ Module::CL_VERSION_12 }, "production", { clcode::base, clcode::vertex_post_processor});
	testCompilation(Module::Options{ Module::CL_VERSION_12, Module::DEBUG }, "debug/testing", { clcode::base, clcode::vertex_post_processor});
}