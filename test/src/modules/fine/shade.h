#pragma once

#include <base/Buffer.h>
#include <base/Module.h>

#include <kernels/base.cl.h>
#include <kernels/fine_rasterizer.cl.h>

#include <rendering/Render.h>

#include <utils/StandardDispatch.h>

#include "fine_utils.h"

using namespace nr;
using namespace nr::detail;
using namespace testing;

struct ShadeTest : public StandardDispatch<1, Fragment, ScreenDimension, Buffer, Buffer>
{
    ShadeTest(Module code, cl_status& err)
        : StandardDispatch(code, "shade_test", err)
    {
    }

	cl_status setFragment(const Fragment& frag)
	{
		return setArg<0>(frag);
	}

	cl_status setScreenDimensions(const ScreenDimension& screenDim)
	{
		return setArg<1>(screenDim);
	}

	cl_status setColorBuffer(const Buffer& color)
	{
		return setArg<2>(color);
	}

	cl_status setDepthBuffer(const Buffer& depth)
	{
		return setArg<3>(depth);
	}

	cl_status setFrameBuffer(const FrameBuffer& frame)
	{
		cl_status ret = setColorBuffer(frame.color);
		if (error::isFailure(ret)) return ret;

		return setDepthBuffer(frame.depth);
	}

	void setExecutionRange(const nr_uint s)
	{
		range.global.x = s;
		range.local.x = s;
	}
};

TEST(Fine, Shade)
{
    cl_status err = CL_SUCCESS;

    const nr_uint dim = 6;
    constexpr ScreenDimension screenDim = { 5, 2 };
    constexpr nr_uint totalScreenSize = screenDim.width * screenDim.height;

	using ColorBuffer = ColorBuffer<screenDim.width, screenDim.height>;
	using DepthBuffer = DepthBuffer<screenDim.width, screenDim.height>;

	ColorBuffer h_color;
	DepthBuffer h_depth;

	for (nr_uint y = 0; y < screenDim.height; ++y)
    {
		for (nr_uint x = 0; x < screenDim.width; ++x)
		{
			h_color[y][x] = { 0, 0, 0, 0 };
			h_depth[y][x] = 1;
		}
    }

    Fragment firstFrag;
    firstFrag.position = { screenDim.width / 5, screenDim.height / 2 };
    firstFrag.color = { 255, 0, 0, 0 };
    firstFrag.depth = 1 / 0.5;

    Fragment secondFrag;
    secondFrag.position = firstFrag.position;
    secondFrag.color = { 255, 0, 0, 0 };
    secondFrag.depth = firstFrag.depth / 1.1f;

    FrameBuffer frame;
    frame.color = Buffer::make<ColorBuffer>(defaultContext, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, 1, &h_color, err);
    ASSERT_SUCCESS(err);
    frame.depth = Buffer::make<DepthBuffer>(defaultContext, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, 1, &h_depth, err);
    ASSERT_SUCCESS(err);
    
    auto code = mkFineModule(err);
    ASSERT_SUCCESS(err);

    auto testee = ShadeTest(code, err);
    ASSERT_SUCCESS(err);

    auto q = defaultCommandQueue;
    ASSERT_SUCCESS(err);

	testee.setExecutionRange(1);

	ASSERT_SUCCESS(testee.setScreenDimensions(screenDim));
	ASSERT_SUCCESS(testee.setFrameBuffer(frame));

    ASSERT_SUCCESS(testee.setFragment(firstFrag));
    ASSERT_SUCCESS(q.enqueueDispatchCommand(testee));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(frame.color, false, 1, &h_color));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(frame.depth, false, 1, &h_depth));
    ASSERT_SUCCESS(q.await());

    EXPECT_EQ(firstFrag.color, h_color[firstFrag.position.y][firstFrag.position.x]);
    EXPECT_EQ(firstFrag.depth, h_depth[firstFrag.position.y][firstFrag.position.x]);

	ASSERT_SUCCESS(testee.setFragment(secondFrag));
	ASSERT_SUCCESS(q.enqueueDispatchCommand(testee));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(frame.color, false, 1, &h_color));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(frame.depth, false, 1, &h_depth));
    ASSERT_SUCCESS(q.await());

	EXPECT_EQ(firstFrag.color, h_color[secondFrag.position.y][secondFrag.position.x]);
	EXPECT_EQ(firstFrag.depth, h_depth[secondFrag.position.y][secondFrag.position.x]);
}
