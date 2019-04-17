#pragma once

#include <inc/includes.h>

#include "bin_utils.h"

#include <base/Module.h>
#include <kernels/base.cl.h>
#include <kernels/bin_rasterizer.cl.h>
#include <pipeline/BinRasterizer.h>
#include <base/Buffer.h>

#include <chrono>

using namespace nr;
using namespace nr::__internal;
using namespace testing;

TEST(Binning, RasterizerOverflow)
{
    cl_int err = CL_SUCCESS; 
    Error error = Error::NO_ERROR;

    const NRuint dim = 4;
    const NRuint simplexCount = 400;

    const NRuint workGroupCount = 2;
    const NRuint binWidth = 2;
    const NRuint binHeight = 2;
    const ScreenDimension screenDim = {4, 10};
    const NRuint binQueueSize = 10;

    const auto q = cl::CommandQueue::getDefault();

    Simplex<dim> h_simplices[simplexCount];
    for (NRuint i = 0; i < simplexCount; ++i)
    {
        mkSimplexInCoords(0, 0, screenDim, h_simplices + i);
    }

    const char options_fmt[] = "-cl-std=CL2.0 -Werror -D _DEBUG -D _TEST_BINNING -D RENDER_DIMENSION=%d -D SIMPLEX_TEST_COUNT=%d";
    char options[sizeof(options_fmt) * 2];
    memset(options, 0, sizeof(options));

    sprintf(options, options_fmt, dim, simplexCount);

    auto code = Module({clcode::base, clcode::bin_rasterizer}, options, &err);
    ASSERT_EQ(CL_SUCCESS, err);

    auto testee = code.makeKernel<BinRasterizerParams>("bin_rasterize", &err);
    ASSERT_EQ(CL_SUCCESS, err);

    Buffer d_simplices(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, simplexCount * sizeof(Simplex<dim>), (float*) h_simplices, &error);
    ASSERT_PRED1(error::isSuccess, error);

    ASSERT_PRED1(error::isSuccess, testee.params.allocateBinQueues(workGroupCount, screenDim, binWidth, binHeight, binQueueSize));
    testee.params.configureSimplexBuffer(d_simplices, simplexCount);

    testee.global = cl::NDRange(workGroupCount * screenDim.width, 1);
    testee.local  = cl::NDRange(screenDim.width, screenDim.height);

    ASSERT_EQ(CL_SUCCESS, testee(q));
    ASSERT_EQ(CL_SUCCESS, q.finish());
    ASSERT_TRUE(testee.params.isOverflowing(q, &err));
    ASSERT_EQ(CL_SUCCESS, err);
}
