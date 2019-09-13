#pragma once

#include "bin_utils.h"

#include <base/Module.h>
#include <base/Buffer.h>
#include <rendering/Render.h>

using namespace nr;
using namespace nr::detail;
using namespace testing;

struct TriangleInBin : StandardDispatch<1, Buffer, Buffer, Bin, ScreenDimension, Buffer>
{
    TriangleInBin(const Module& module, cl_status& err)
        : StandardDispatch(module, "is_triangle_in_bin_test", err)
    {
    }

	cl_status setTriangleXBuffer(const Buffer& buffer)
	{
		return setArg<0>(buffer);
	}

	cl_status setTriangleYBuffer(const Buffer& buffer)
	{
		return setArg<1>(buffer);
	}

	cl_status setBin(const Bin& bin)
	{
		return setArg<2>(bin);
	}

	cl_status setScreenDimension(const ScreenDimension& screenDim)
	{
		return setArg<3>(screenDim);
	}

	cl_status setResultBuffer(const Buffer& result)
	{
		return setArg<4>(result);
	}

	void setExecutionRange(const nr_uint count)
	{
		range.global.x = count;
		range.local.x = 1;
	}
};

void testBin(TriangleInBin testee, const CommandQueue& q, const Bin& bin, const ScreenDimension& dim, nr_float* triangle_x, nr_float* triangle_y)
{
    cl_status err = CL_SUCCESS;

    cl_bool h_result = CL_FALSE;
    auto d_result = Buffer::make(defaultContext, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, 1, &h_result, err);
    ASSERT_SUCCESS(err);

    nr_float defaultValue = -2.0;
    nr_int defaultValueAsInt = * (nr_int*) &defaultValue; 
    memset(triangle_x, defaultValueAsInt, 3 * sizeof(nr_float));
    memset(triangle_y, defaultValueAsInt, 3 * sizeof(nr_float));
    auto d_triangle_x = Buffer::make<nr_float>(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 3, triangle_x, err);
    ASSERT_SUCCESS(err);
    auto d_triangle_y = Buffer::make<nr_float>(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 3, triangle_x, err);
    ASSERT_SUCCESS(err);

	testee.setExecutionRange(1);
    
    // Shouldn't be in any bin
	ASSERT_SUCCESS(testee.setBin(bin));
	ASSERT_SUCCESS(testee.setScreenDimension(dim));
	ASSERT_SUCCESS(testee.setTriangleXBuffer(d_triangle_x));
	ASSERT_SUCCESS(testee.setTriangleYBuffer(d_triangle_y));
	ASSERT_SUCCESS(testee.setResultBuffer(d_result));
    ASSERT_SUCCESS(q.enqueueDispatchCommand(testee));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_result, false, 1, &h_result));
    ASSERT_SUCCESS(q.await());

    // Middle of bin
    mkTriangleInCoords(bin.x + bin.width / 2, bin.y + bin.height / 2, dim, triangle_x, triangle_y);
    ASSERT_SUCCESS(q.enqueueBufferWriteCommand(d_triangle_x, false, 3, triangle_x));
    ASSERT_SUCCESS(q.enqueueBufferWriteCommand(d_triangle_y, false, 3, triangle_y));
	ASSERT_SUCCESS(q.enqueueDispatchCommand(testee));
	ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_result, false, 1, &h_result));
    ASSERT_SUCCESS(q.await());

    // Left-Low corner
    mkTriangleInCoords(bin.x, bin.y, dim, triangle_x, triangle_y);
    ASSERT_SUCCESS(q.enqueueBufferWriteCommand(d_triangle_x, false, 3, triangle_x));
    ASSERT_SUCCESS(q.enqueueBufferWriteCommand(d_triangle_y, false, 3, triangle_y));
    ASSERT_SUCCESS(q.await());
	ASSERT_SUCCESS(q.enqueueDispatchCommand(testee));
	ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_result, false, 1, &h_result));
    ASSERT_SUCCESS(q.await());
}

TEST(Binning, IsSimplexInBin)
{
    cl_status err = CL_SUCCESS; 

    nr_float triangle_x[3];
    nr_float triangle_y[3];

    auto code = mkBinningModule(1, err);
    ASSERT_SUCCESS(err);

    auto test = TriangleInBin(code, err);
    ASSERT_SUCCESS(err);

    const ScreenDimension screenDim{1920, 1080};
    const Bin bin{32, 32, 800, 800};

    testBin(test, defaultCommandQueue, bin, screenDim, triangle_x, triangle_y);
}

