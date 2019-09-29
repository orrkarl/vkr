#pragma once

#include <base/Module.h>
#include <kernels/base.cl.h>
#include <kernels/fine_rasterizer.cl.h>
#include <base/Buffer.h>
#include <rendering/Render.h>
#include <pipeline/BinRasterizer.h>
#include <pipeline/FineRasterizer.h>
#include "fine_utils.h"

using namespace nr;
using namespace nr::detail;
using namespace testing;

TEST(Fine, Depth)
{
    cl_status err = CL_SUCCESS;

	constexpr const nr_uint point_count = 4;
    constexpr const ScreenDimension screenDim = { 1366, 768 };
    constexpr const nr_uint totalScreenSize = screenDim.width * screenDim.height;
    constexpr const BinQueueConfig config = { 32, 32, 256 };
    constexpr const nr_uint binCountX = compileTimeCeil(((nr_float) screenDim.width) / config.binWidth);
    constexpr const nr_uint binCountY = compileTimeCeil(((nr_float) screenDim.height) / config.binHeight);

	using Queues = BinQueues<binCountX, binCountY, config.queueSize>;
	using ColorBuffer = ColorBuffer<screenDim.width, screenDim.height>;
	using DepthBuffer = DepthBuffer<screenDim.width, screenDim.height>;

    const nr_uint totalBinCount = binCountX * binCountY;
    const nr_uint totalWorkGroupCount = 6;
    const nr_uint totalBinQueuesSize = totalWorkGroupCount * totalBinCount * (config.queueSize + 1); 

    const nr_float defaultDepth = 1;
    const nr_float expectedDepth = 0.5;

    const nr_uint triangleCount = 3 * totalBinCount;
    auto h_records = std::make_unique<TriangleRecord[]>(triangleCount);
	auto h_binQueues = std::make_unique<Queues[]>(totalWorkGroupCount);

	std::unique_ptr<ColorBuffer[]> cBuffer(new ColorBuffer[1]);
    std::unique_ptr<DepthBuffer[]> dBuffer(new DepthBuffer[1]);

	auto h_colorBuffer = cBuffer[0];
	auto h_depthBuffer = dBuffer[0];
	
    Fragment expected;
    expected.color = RED;

	for (nr_uint y = 0; y < screenDim.height; ++y)
    {
		for (nr_uint x = 0; x < screenDim.width; ++x)
		{
			h_colorBuffer[y][x] = { 0, 0, 0, 0 };
			h_depthBuffer[y][x] = defaultDepth;
		}
    }
    
    fillTriangles<binCountX, binCountY, config.queueSize>(screenDim, config, totalWorkGroupCount, expectedDepth, 256, h_records.get(), h_binQueues.get());
	
    auto code = mkFineModule(err);
    ASSERT_SUCCESS(err);
	
    auto testee = FineRasterizer(code, err);
    ASSERT_SUCCESS(err);
	
    auto q = defaultCommandQueue;
	
    FrameBuffer frame;
    frame.color = Buffer::make<ColorBuffer>(defaultContext, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, 1, cBuffer.get(), err);
    ASSERT_SUCCESS(err);
    frame.depth = Buffer::make<DepthBuffer>(defaultContext, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 1, dBuffer.get(), err);
    ASSERT_SUCCESS(err);
	
    auto d_triangles = Buffer::make<TriangleRecord>(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, triangleCount, h_records.get(), err);
    ASSERT_SUCCESS(err);
    auto d_binQueues = Buffer::make<Queues>(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, totalWorkGroupCount, h_binQueues.get(), err);
    ASSERT_SUCCESS(err);
	
	testee.setExecutionRange(binCountX, binCountY, totalWorkGroupCount);

	ASSERT_SUCCESS(testee.setTriangleRecordInputBuffer(d_triangles));
	ASSERT_SUCCESS(testee.setScreenDimensions(screenDim));
	ASSERT_SUCCESS(testee.setBinQueuesConfig(config));
	ASSERT_SUCCESS(testee.setBinQueuesBuffer(d_binQueues));
	ASSERT_SUCCESS(testee.setBinningWorkGroupCount(totalWorkGroupCount));
	ASSERT_SUCCESS(testee.setFrameBuffer(frame));
    ASSERT_SUCCESS(q.enqueueDispatchCommand(testee));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(frame.color, false, 1, cBuffer.get()));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(frame.depth, false, 1, dBuffer.get()));
	ASSERT_SUCCESS(q.finish());
    ASSERT_TRUE((validateDepth<screenDim.width, screenDim.height>(h_depthBuffer, defaultDepth, expectedDepth)));
}
