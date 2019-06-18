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

    const NRuint dim = 4;
    const NRuint triangleCount = 400;

    const NRuint workGroupCount = 2;
    const ScreenDimension screenDim = {4, 10};
    const BinQueueConfig config = {32, 32, 10};

    NRbool isOverflowing = false;

    const auto q = cl::CommandQueue::getDefault();

    Triangle<dim> h_triangles[triangleCount];
    for (NRuint i = 0; i < triangleCount; ++i)
    {
        mkTriangleInCoords(0, 0, screenDim, h_triangles + i);
    }
    
    auto code = mkBinningModule(dim, triangleCount, &err);
    ASSERT_EQ(CL_SUCCESS, err);

    auto testee = code.makeKernel<BinRasterizerParams>("bin_rasterize", &err);
    ASSERT_EQ(CL_SUCCESS, err);
    
    Buffer d_triangles(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, triangleCount * sizeof(Triangle<dim>), (float*) h_triangles, &err);
    ASSERT_PRED1(error::isSuccess, err);

    Buffer d_overflow(CL_MEM_READ_WRITE, sizeof(cl_bool), nullptr, &err);
    ASSERT_PRED1(error::isSuccess, err);

    Buffer d_binQueues(CL_MEM_READ_WRITE, BinRasterizerParams::getTotalBinQueueSize(workGroupCount, screenDim, config), &err);
    ASSERT_PRED1(error::isSuccess, err);

    Buffer d_batchIndex(CL_MEM_READ_WRITE, sizeof(cl_uint), &err);
    ASSERT_PRED1(error::isSuccess, err);

    testee.params.binQueueConfig = config;
    testee.params.dimension = screenDim;
    testee.params.triangleData = d_triangles;
    testee.params.triangleCount = triangleCount;
    testee.params.hasOverflow = d_overflow;
    testee.params.batchIndex = d_batchIndex;

    testee.global = cl::NDRange(workGroupCount * screenDim.width, screenDim.height);
    testee.local  = cl::NDRange(screenDim.width, screenDim.height);

    ASSERT_EQ(CL_SUCCESS, testee(q));
    ASSERT_EQ(CL_SUCCESS, q.enqueueReadBuffer(d_overflow, CL_FALSE, 0, sizeof(isOverflowing), &isOverflowing));
    ASSERT_EQ(CL_SUCCESS, q.finish());
    ASSERT_TRUE(isOverflowing);
    ASSERT_EQ(CL_SUCCESS, err);
}
