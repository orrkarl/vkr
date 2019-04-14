#include "../includes.h"

#include <base/Module.h>
#include <kernels/base.cl.h>
#include <kernels/bin_rasterizer.cl.h>

using namespace nr;
using namespace nr::__internal;

TEST(Binning, Compilation)
{
    cl_int err = CL_SUCCESS; 

    Module base({clcode::base, clcode::bin_rasterizer}, "-D RenderDimension=3", &err);
    ASSERT_EQ(CL_SUCCESS, err);

    auto log = base.getBuildLog(&err);
    ASSERT_EQ(CL_SUCCESS, err);
    ASSERT_EQ("", log) << log;
}