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

// Because MSVC doesn't evaluate constexpr as well as GCC...
constexpr nr_uint compileTimeCeil(const nr_float f)
{
	const nr_uint as_uint = static_cast<nr_uint>(f);
	return as_uint == f ? as_uint : as_uint + 1;
}

TEST(Binning, Rasterizer)
{
    cl_status err = CL_SUCCESS; 

    const nr_uint dim = 4;
    
    // Don't change this - test not generic enough
    const nr_uint triangleCount = 2;

    // Don't change this, may be critical for the test's determinism
	constexpr nr_uint workGroupCount = 1;

	constexpr ScreenDimension screenDim = {64, 64};
    constexpr BinQueueConfig config = {32, 32, 10};

	constexpr nr_uint binCountX = compileTimeCeil(((nr_float) screenDim.width) / config.binWidth);
	constexpr nr_uint binCountY = compileTimeCeil(((nr_float) screenDim.height) / config.binHeight);

	nr_uint h_result[workGroupCount * (config.queueSize + 1) * binCountX * binCountY];

    nr_bool isOverflowing = false;

    const nr_uint destinationBinX = 1;
    const nr_uint destinationBinY = 1;
    const nr_uint destinationBinQueueRawIndex = (config.queueSize + 1) * (destinationBinY * binCountX + destinationBinX);
    const nr_uint* destBinQueueBase = h_result + destinationBinQueueRawIndex;

    const nr_uint x = destinationBinX * config.binWidth + config.binWidth / 2;
    const nr_uint y = destinationBinY * config.binHeight + config.binHeight / 2;
    Triangle<dim> h_triangles[triangleCount];
    mkTriangleInCoords(x, y, screenDim, h_triangles);
    mkTriangleInCoords(x, y, screenDim, h_triangles + 1);
    
    CommandQueue q = CommandQueue::getDefault();

    auto code = mkBinningModule(dim, triangleCount, &err);
    ASSERT_SUCCESS(err);

    auto testee = BinRasterizer(code, &err);
    ASSERT_SUCCESS(err);

    Buffer<nr_float> d_triangles(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(h_triangles) / sizeof(nr_float), (nr_float*) h_triangles, &err);
    ASSERT_SUCCESS(err);

    Buffer<nr_bool> d_overflow(CL_MEM_READ_WRITE, 1, &err);
    ASSERT_SUCCESS(err);

    Buffer<nr_uint> d_binQueues(CL_MEM_READ_WRITE, BinRasterizer::getTotalBinQueueCount(workGroupCount, screenDim, config), &err);
    ASSERT_SUCCESS(err);

    Buffer<nr_uint> d_batchIndex(CL_MEM_READ_WRITE, 1, &err);
    ASSERT_SUCCESS(err);

    testee.binQueueConfig = config;
    testee.dimension      = screenDim;
    testee.triangleData   = d_triangles;
    testee.triangleCount  = triangleCount;
    testee.hasOverflow    = d_overflow;
    testee.binQueues      = d_binQueues;
    testee.batchIndex     = d_batchIndex;

    std::array<size_t, 2> global = { workGroupCount * binCountX, binCountY };
    std::array<size_t, 2> local  = { binCountX, binCountY };

    ASSERT_SUCCESS(testee.load());
    ASSERT_SUCCESS(q.enqueueKernelCommand<2>(testee, global, local));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_binQueues, false, sizeof(h_result) / sizeof(nr_float), h_result));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_overflow, false, sizeof(isOverflowing) / sizeof(nr_uint), &isOverflowing));
    ASSERT_SUCCESS(q.await());
    ASSERT_FALSE(isOverflowing);

    ASSERT_EQ(0, h_result[0]);  // queue for bin (0, 0) is not empty
    ASSERT_EQ(0, h_result[1]);  // the first triangle is in it
    ASSERT_EQ(1, h_result[2]);  // the second triangle is in it
    ASSERT_EQ(0, h_result[3]);  // no more triangles

    ASSERT_EQ(0, destBinQueueBase[0]); // the designated queue is not empty  
    ASSERT_EQ(0, destBinQueueBase[1]); // the queue's first element points to the first triangle
    ASSERT_EQ(1, destBinQueueBase[2]); // the queue's second element points to the second triangle
    ASSERT_EQ(0, destBinQueueBase[3]); // queue end
}