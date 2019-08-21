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

// Don't run this test with high screen dimensions as floating point impercision renders it useless there
TEST(Fine, Rasterizer)
{
	cl_status err = CL_SUCCESS;

	constexpr const nr_uint dim = 5;
	constexpr const ScreenDimension screenDim = { 640, 640 };
	constexpr const BinQueueConfig config = { 64, 64, 10 };
	constexpr const nr_uint binCountX = compileTimeCeil(((nr_float)screenDim.width) / config.binWidth);
	constexpr const nr_uint binCountY = compileTimeCeil(((nr_float)screenDim.height) / config.binHeight);
	
	using Queues = BinQueues<binCountX, binCountY, config.queueSize>;
	using ColorBuffer = ColorBuffer<screenDim.width, screenDim.height>;
	using DepthBuffer = DepthBuffer<screenDim.width, screenDim.height>;
	
	const nr_uint totalWorkGroupCount = 1;

	const nr_float defaultDepth = 1;
	const nr_float expectedDepth = 0.5f;

	const nr_uint triangleCount = 3;
	std::unique_ptr<Triangle<dim>[]> h_triangles(new Triangle<dim>[triangleCount]);
	std::unique_ptr<Queues[]> h_binQueues(new Queues[totalWorkGroupCount]);

	std::unique_ptr<ColorBuffer[]> h_colorBuffer(new ColorBuffer[1]);
	std::unique_ptr<DepthBuffer[]> h_depthBuffer(new DepthBuffer[1]);

	std::unique_ptr<ColorBuffer[]> expectedColorBuffer(new ColorBuffer[1]);
	std::unique_ptr<DepthBuffer[]> expectedDepthBuffer(new DepthBuffer[1]);

	for (auto y = 0u; y < screenDim.height; ++y)
	{
		for (auto x = 0u; x < screenDim.width; ++x)
		{
			h_depthBuffer[0][y][x] = defaultDepth;
			h_colorBuffer[0][y][x] = { 0, 0, 0, 0 };
			
			expectedColorBuffer[0][y][x] = { 0, 0, 0, 0 };
			expectedDepthBuffer[0][y][x] = defaultDepth;
		}
	}

	tesselateScreen<dim, binCountX, binCountY, config.queueSize, screenDim.width, screenDim.height>(
		screenDim, config, totalWorkGroupCount, expectedDepth, triangleCount, h_triangles.get(), h_binQueues.get(), expectedColorBuffer[0], expectedDepthBuffer[0]);

	auto code = mkFineModule(dim, &err);
	ASSERT_SUCCESS(err);

	auto testee = FineRasterizer(code, &err);
	ASSERT_SUCCESS(err);

	auto q = defaultCommandQueue;

	FrameBuffer frame;
	frame.color = Buffer::make<ColorBuffer>(defaultContext, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, 1, h_colorBuffer.get(), &err);
	ASSERT_SUCCESS(err);
	frame.depth = Buffer::make<DepthBuffer>(defaultContext, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 1, h_depthBuffer.get(), &err);
	ASSERT_SUCCESS(err);

	auto d_triangles = Buffer::make<Triangle<dim>>(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, triangleCount, h_triangles.get(), &err);
	ASSERT_SUCCESS(err);
	auto d_binQueues = Buffer::make<Queues>(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, totalWorkGroupCount, h_binQueues.get(), &err);
	ASSERT_SUCCESS(err);

	testee.setExecutionRange(binCountX, binCountY, totalWorkGroupCount);
	
	ASSERT_SUCCESS(testee.setTriangleInputBuffer(d_triangles));
	ASSERT_SUCCESS(testee.setScreenDimensions(screenDim));
	ASSERT_SUCCESS(testee.setBinQueuesConfig(config));
	ASSERT_SUCCESS(testee.setBinQueuesBuffer(d_binQueues));
	ASSERT_SUCCESS(testee.setBinningWorkGroupCount(totalWorkGroupCount));
	ASSERT_SUCCESS(testee.setFrameBuffer(frame));
	ASSERT_SUCCESS(q.enqueueDispatchCommand(testee));
	ASSERT_SUCCESS(q.enqueueBufferReadCommand(frame.color, false, 1, h_colorBuffer.get()));
	ASSERT_SUCCESS(q.enqueueBufferReadCommand(frame.depth, false, 1, h_depthBuffer.get()));
	ASSERT_SUCCESS(q.await());
	EXPECT_TRUE((validateBuffer<nr::RawColorRGBA, screenDim.width, screenDim.height>("color", expectedColorBuffer[0], h_colorBuffer[0])));
	EXPECT_TRUE((validateBuffer<screenDim.width, screenDim.height>("depth", expectedDepthBuffer[0], h_depthBuffer[0])));
}
