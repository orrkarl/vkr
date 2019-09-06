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

struct PointInTriangle : StandardDispatch<1, Triangle, ScreenPosition, ScreenDimension, Buffer>
{    
    PointInTriangle(const Module& code, cl_status& err)
        : StandardDispatch(code, "is_point_in_triangle_test", err)
    {
    }

	cl_status setTriangleInputBuffer(const Triangle& tri)
	{
		return setArg<0>(tri);
	}

	cl_status setPosition(const ScreenPosition& pos)
	{
		return setArg<1>(pos);
	}

	cl_status setDimension(const ScreenDimension& dim)
	{
		return setArg<2>(dim);
	}

	cl_status setResultBuffer(const Buffer& res)
	{
		return setArg<3>(res);
	}

	void setExecutionRange(const nr_uint s)
	{
		range.global.x = s;
		range.local.x = s;
	}
};

TEST(Fine, PointInTriangle)
{
    cl_status err = CL_SUCCESS;

    Triangle triangle;
    const NDCPosition left_bottom  = { -0.5, -0.5 };
    const NDCPosition right_bottom = { 0.5, -0.5 };
    const NDCPosition top          = { 0, 0.5 };
    mkTriangleInCoordinates(left_bottom, top, right_bottom, triangle);

    constexpr const ScreenDimension screenDim  = { 1920, 1080 };
    const ScreenPosition origin      = { (nr_uint) (0.5 * 1920), (nr_uint) (0.5 * 1080) };
    const ScreenPosition belowBottom = { (nr_uint) (0.5 * 1920), (nr_uint) (0.1 * 1080) };
    const ScreenPosition justInside  = { (nr_uint) (0.65 * 1920), (nr_uint) (0.4 * 1080) };

    nr_bool h_result = false;

    auto q = defaultCommandQueue;

    auto code = mkFineModule(err);
    ASSERT_SUCCESS(err);

    auto testee = PointInTriangle(code, err);
    ASSERT_SUCCESS(err);

    auto d_result = Buffer::make<nr_bool>(defaultContext, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 1, &h_result, err);
    ASSERT_SUCCESS(err);

	testee.setExecutionRange(1);

	ASSERT_SUCCESS(testee.setTriangleInputBuffer(triangle));
	ASSERT_SUCCESS(testee.setDimension(screenDim));
	ASSERT_SUCCESS(testee.setResultBuffer(d_result));

    ASSERT_SUCCESS(testee.setPosition(origin));
    ASSERT_SUCCESS(q.enqueueDispatchCommand(testee));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_result, false, 1, &h_result));
    ASSERT_SUCCESS(q.await());
    EXPECT_TRUE(h_result);

	ASSERT_SUCCESS(testee.setPosition(belowBottom));
	ASSERT_SUCCESS(q.enqueueDispatchCommand(testee));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_result, false, 1, &h_result));
    ASSERT_SUCCESS(q.await());
    EXPECT_FALSE(h_result);

	ASSERT_SUCCESS(testee.setPosition(justInside));
	ASSERT_SUCCESS(q.enqueueDispatchCommand(testee));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_result, false, 1, &h_result));
    ASSERT_SUCCESS(q.await());
	EXPECT_TRUE(h_result);
}
