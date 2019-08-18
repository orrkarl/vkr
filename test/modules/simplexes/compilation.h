#include <kernels/simplex_reducing.cl.h>

#include "../../includes.h"

using namespace nr;
using namespace testing;

TEST(SimplexReducer, Compilation)
{
	testCompilation(Module::Options{ Module::CL_VERSION_12, Module::RenderDimension(11) }, "production 11d", { clcode::base, clcode::simplex_reducing });
	testCompilation(Module::Options{ Module::CL_VERSION_12, Module::RenderDimension(3), Module::DEBUG }, "debug/testing 3d", { clcode::base, clcode::simplex_reducing });
}

