#pragma once

#include <base/Buffer.h>
#include <base/Module.h>

#include <kernels/base.cl.h>
#include <kernels/bin_rasterizer.cl.h>

#include <utils/StandardDispatch.h>

#include "bin_utils.h"

using namespace nr;
using namespace nr::detail;
using namespace testing;


class ReduceTriangleBuffer : public StandardDispatch<1, Buffer, nr_uint, Buffer>
{
public:
    ReduceTriangleBuffer(const Module& module, cl_status& err)
        : StandardDispatch(module, "reduce_triangle_buffer_test", err)
    {
    }

	cl_status setTrianglesInputBuffer(const Buffer& buffer)
	{
		return setArg<0>(buffer);
	}

	cl_status setTriangleOffset(const nr_uint offset)
	{
		return setArg<1>(offset);
	}

	cl_status setResultBuffer(const Buffer& buffer)
	{
		return setArg<2>(buffer);
	}

	void setExecutionRange(const nr_uint groupSize)
	{
		range.global.x = groupSize;
		range.local.x = groupSize;
	}
};

struct ReducedTriangle
{
	NDCPosition p0, p1, p2;

	bool operator==(const ReducedTriangle& other) const
	{
		return p0 == other.p0 && p1 == other.p1 && p2 == other.p2;
	}

	friend std::ostream& operator<<(std::ostream& os, const ReducedTriangle& tri)
	{
		return os << "ReducesTriangle{" << tri.p0 << ", " << tri.p1 << ", " << tri.p2 << "}";
	}
};

void generateTriangleData(const nr_uint triangleCount, Triangle* buffer)
{
    nr_float diff = 2.0f / (triangleCount - 1);
    nr_float base = -1;

    for (nr_uint i = 0; i < triangleCount; ++i)
    {
        for (nr_uint j = 0; j < 3; ++j)
        {
            buffer[i].points[j].values[0] = base + i * diff;
            buffer[i].points[j].values[1] = base + i * diff;

			buffer[i].points[j].values[2] = -2;
        }
    }
}

void extractNDCPosition(const nr_uint triangleCount, const Triangle* buffer, ReducedTriangle* result)
{
    for (nr_uint i = 0; i < triangleCount; ++i)
    {
		result[i].p0.x = buffer[i][0][0];
		result[i].p0.y = buffer[i][0][1];
		
		result[i].p1.x = buffer[i][1][0];
		result[i].p1.y = buffer[i][1][1];

		result[i].p2.x = buffer[i][2][0];
		result[i].p2.y = buffer[i][2][1];
    }
}


TEST(Binning, ReduceTriangleBuffer)
{
    const nr_uint triangleCount = 3;
    const nr_uint offset = 2;
	const nr_uint workGroupSize = 30;
    
    cl_status err = CL_SUCCESS; 

    auto code = mkBinningModule(triangleCount + offset, err);
    ASSERT_SUCCESS(err);

    auto q = defaultCommandQueue;

    Triangle h_triangles_raw[triangleCount + offset];
    Triangle* h_triangles = h_triangles_raw + offset;

    generateTriangleData(triangleCount, h_triangles);
	
	ReducedTriangle h_expected[triangleCount];
    extractNDCPosition(triangleCount, h_triangles, h_expected);

    std::array<ReducedTriangle, triangleCount> h_actual;

    auto d_triangle = Buffer::make<Triangle>(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, offset + triangleCount, h_triangles_raw, err);
    ASSERT_SUCCESS(err);
    auto d_result = Buffer::make<ReducedTriangle>(defaultContext, CL_MEM_WRITE_ONLY, triangleCount, err);
    ASSERT_SUCCESS(err);
    
    auto test = ReduceTriangleBuffer(code, err);
    ASSERT_SUCCESS(err);

	test.setExecutionRange(workGroupSize);

	ASSERT_SUCCESS(test.setTrianglesInputBuffer(d_triangle));
	ASSERT_SUCCESS(test.setTriangleOffset(offset));
	ASSERT_SUCCESS(test.setResultBuffer(d_result));
    ASSERT_SUCCESS(q.enqueueDispatchCommand(test));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_result, false, triangleCount, h_actual.data()));
    ASSERT_SUCCESS(q.await());

    ASSERT_THAT(h_actual, ElementsAreArray(h_expected, triangleCount));
}

