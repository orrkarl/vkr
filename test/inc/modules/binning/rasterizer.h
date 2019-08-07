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
using namespace nr::detail;
using namespace testing;

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

	using Queues = BinQueues<binCountX, binCountY, config.queueSize>;

	constexpr const nr_uint totalBinCount = binCountX * binCountY;

	Queues h_result[workGroupCount];

	//nr_uint h_result[workGroupCount * (config.queueSize + 1) * binCountX * binCountY];

    nr_bool isOverflowing = false;

    const nr_uint destinationBinX = 1;
    const nr_uint destinationBinY = 1;
    const BinQueue<config.queueSize>& destBinQueueBase = h_result[0][destinationBinX][destinationBinY];

    const nr_uint x = destinationBinX * config.binWidth + config.binWidth / 2;
    const nr_uint y = destinationBinY * config.binHeight + config.binHeight / 2;
    Triangle<dim> h_triangles[triangleCount];
    mkTriangleInCoords(x, y, screenDim, h_triangles);
    mkTriangleInCoords(x, y, screenDim, h_triangles + 1);
    
    CommandQueue q = defaultCommandQueue;

    auto code = mkBinningModule(dim, triangleCount, &err);
    ASSERT_SUCCESS(err);

    auto testee = BinRasterizer(code, &err);
    ASSERT_SUCCESS(err);

    auto d_triangles = Buffer::make<Triangle<dim>>(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, triangleCount, h_triangles, &err);
    ASSERT_SUCCESS(err);

    auto d_overflow = Buffer::make<nr_uint>(defaultContext, CL_MEM_READ_WRITE, 1, &err);
    ASSERT_SUCCESS(err);

    auto d_binQueues = Buffer::make<nr_uint>(defaultContext, CL_MEM_READ_WRITE, BinRasterizer::getTotalBinQueueCount(workGroupCount, screenDim, config), &err);
    ASSERT_SUCCESS(err);

    auto d_batchIndex = Buffer::make<nr_uint>(defaultContext, CL_MEM_READ_WRITE, 1, &err);
    ASSERT_SUCCESS(err);

    testee.binQueueConfig = config;
    testee.dimension      = screenDim;
    testee.triangleData   = d_triangles;
    testee.triangleCount  = triangleCount;
    testee.hasOverflow    = d_overflow;
    testee.binQueues      = d_binQueues;
    testee.batchIndex     = d_batchIndex;

    NDRange<2> global = { workGroupCount * binCountX, binCountY };
    NDRange<2> local  = { binCountX, binCountY };

    ASSERT_SUCCESS(testee.load());
    ASSERT_SUCCESS(q.enqueueKernelCommand<2>(testee, global, local));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_binQueues, false, workGroupCount, h_result));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_overflow, false, 1, &isOverflowing));
    ASSERT_SUCCESS(q.await());
    ASSERT_FALSE(isOverflowing);

    ASSERT_FALSE(h_result[0][0][0].isEmpty);  // queue for bin (0, 0) is not empty
    ASSERT_EQ(0, h_result[0][0][0][0]);  // the first triangle is in it
    ASSERT_EQ(1, h_result[0][0][0][1]);  // the second triangle is in it
    ASSERT_EQ(0, h_result[0][0][0][2]);  // no more triangles

	ASSERT_FALSE(destBinQueueBase.isEmpty); // the designated queue is not empty  
	ASSERT_EQ(0, destBinQueueBase[0]); // the queue's first element points to the first triangle
	ASSERT_EQ(1, destBinQueueBase[1]); // the queue's second element points to the second triangle
	ASSERT_EQ(0, destBinQueueBase[2]); // queue end
}