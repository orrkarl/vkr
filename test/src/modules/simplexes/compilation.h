#include <kernels/simplex_reducer.cl.h>

#include "../../includes.h"

using namespace nr;
using namespace testing;

TEST(SimplexReducer, Compilation)
{
	testCompilation(Module::Options{ Module::CL_VERSION_12 }, "production", { clcode::base, clcode::simplex_reduce });
	testCompilation(Module::Options{ Module::CL_VERSION_12, Module::DEBUG }, "debug/testing", { clcode::base, clcode::simplex_reduce });
}

