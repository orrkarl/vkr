#pragma once

#include "bin_utils.h"

#include <base/Module.h>
#include <pipeline/BinRasterizer.h>
#include <base/Buffer.h>

#include <chrono>
#include <memory>

using namespace nr;
using namespace nr::detail;
using namespace testing;

TEST(Binning, RasterizerOverflow)
{
    cl_status err = CL_SUCCESS; 

    constexpr const nr_uint dim = 4;
    constexpr const nr_uint triangleCount = 400;
    constexpr const nr_uint workGroupCount = 2;
    constexpr const ScreenDimension screenDim = {4, 10};
    constexpr const BinQueueConfig config = {32, 32, 10};

	constexpr nr_uint binCountX = compileTimeCeil(((nr_float)screenDim.width) / config.binWidth);
	constexpr nr_uint binCountY = compileTimeCeil(((nr_float)screenDim.height) / config.binHeight);

	using Queues = BinQueues<binCountX, binCountY, config.queueSize>;

    nr_bool isOverflowing = false;

    auto q = defaultCommandQueue;

	std::unique_ptr<Triangle<dim>[]> h_triangles_(new Triangle<dim>[triangleCount]);
	auto h_triangles = h_triangles_.get();
    for (nr_uint i = 0; i < triangleCount; ++i)
    {
        mkTriangleInCoords(0, 0, screenDim, h_triangles + i);
    }
    
    auto code = mkBinningModule(dim, triangleCount, err);
    ASSERT_SUCCESS(err);

    auto testee = BinRasterizer(code, err);
    ASSERT_SUCCESS(err);
    
    auto d_triangles = Buffer::make<Triangle<dim>>(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, triangleCount, h_triangles, err);
    ASSERT_SUCCESS(err);

    auto d_overflow = Buffer::make<nr_uint>(defaultContext, CL_MEM_READ_WRITE, 1, err);
    ASSERT_SUCCESS(err);

    auto d_binQueues = Buffer::make<Queues>(defaultContext, CL_MEM_READ_WRITE, workGroupCount, err);
    ASSERT_SUCCESS(err);

    auto d_batchIndex = Buffer::make<nr_uint>(defaultContext, CL_MEM_READ_WRITE, 1, err);
    ASSERT_SUCCESS(err);

	testee.setExecutionRange(binCountX, binCountY, workGroupCount);

	ASSERT_SUCCESS(testee.setBinQueueConfig(config));
	ASSERT_SUCCESS(testee.setScreenDimension(screenDim));
	ASSERT_SUCCESS(testee.setTriangleInputBuffer(d_triangles));
	ASSERT_SUCCESS(testee.setTriangleCount(triangleCount));
	ASSERT_SUCCESS(testee.setOvereflowNotifier(d_overflow));
	ASSERT_SUCCESS(testee.setBinQueuesBuffer(d_binQueues));
	ASSERT_SUCCESS(testee.setGlobalBatchIndex(d_batchIndex));
	ASSERT_SUCCESS(q.enqueueDispatchCommand(testee));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_overflow, false, 1, &isOverflowing));
    ASSERT_SUCCESS(q.await());
    ASSERT_TRUE(isOverflowing);
}
