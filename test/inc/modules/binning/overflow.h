#pragma once

#include <inc/includes.h>

#include "bin_utils.h"

#include <base/Module.h>
#include <pipeline/BinRasterizer.h>
#include <base/Buffer.h>

#include <chrono>

using namespace nr;
using namespace nr::__internal;
using namespace testing;

TEST(Binning, RasterizerOverflow)
{
    cl_status err = CL_SUCCESS; 

    const NRuint dim = 4;
    const NRuint triangleCount = 400;

    const NRuint workGroupCount = 2;
    const ScreenDimension screenDim = {4, 10};
    const BinQueueConfig config = {32, 32, 10};

    NRbool isOverflowing = false;

    auto q = CommandQueue::getDefault();

    Triangle<dim> h_triangles[triangleCount];
    for (NRuint i = 0; i < triangleCount; ++i)
    {
        mkTriangleInCoords(0, 0, screenDim, h_triangles + i);
    }
    
    auto code = mkBinningModule(dim, triangleCount, &err);
    ASSERT_SUCCESS(err);

    auto testee = BinRasterizer(code, &err);
    ASSERT_SUCCESS(err);
    
    Buffer<NRfloat> d_triangles(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, triangleCount * sizeof(Triangle<dim>) / sizeof(NRfloat), (NRfloat*) h_triangles, &err);
    ASSERT_SUCCESS(err);

    Buffer<NRbool> d_overflow(CL_MEM_READ_WRITE, 1, &err);
    ASSERT_SUCCESS(err);

    Buffer<NRuint> d_binQueues(CL_MEM_READ_WRITE, BinRasterizer::getTotalBinQueueCount(workGroupCount, screenDim, config), &err);
    ASSERT_SUCCESS(err);

    Buffer<NRuint> d_batchIndex(CL_MEM_READ_WRITE, 1, &err);
    ASSERT_SUCCESS(err);

    testee.binQueueConfig = config;
    testee.dimension = screenDim;
    testee.triangleData = d_triangles;
    testee.triangleCount = triangleCount;
    testee.hasOverflow = d_overflow;
    testee.batchIndex = d_batchIndex;

    std::array<size_t, 2> global = { workGroupCount * screenDim.width, screenDim.height };
    std::array<size_t, 2> local  = { screenDim.width, screenDim.height };

    ASSERT_SUCCESS(testee.load());
    ASSERT_SUCCESS(q.enqueueKernelCommand<2>(testee, global, local));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_overflow, false, 1, &isOverflowing));
    ASSERT_SUCCESS(q.await());
    ASSERT_TRUE(isOverflowing);
}
