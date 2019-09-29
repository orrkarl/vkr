#pragma once

#include <base/Module.h>
#include <base/Buffer.h>
#include <kernels/base.cl.h>
#include <kernels/vertex_post_processor.cl.h>
#include <pipeline/VertexPostProcessor.h>
#include <rendering/Render.h>
#include <utils/StandardDispatch.h>

#include "../../../includes.h"

using namespace nr;
using namespace testing;

TEST(VertexPostProcessing, Processing)
{
	cl_status err = CL_SUCCESS;
	constexpr nr_uint count = 5;

	Triangle triangles[count] =
	{
		{	// Degenerate triangle
			Vertex(0.0f, 0.0f, 0.0f),
			Vertex(1.0f, 1.0f, 1.0f),
			Vertex(2.0f, 2.0f, 2.0f)
		},
		{	// CCW triangle
			Vertex(1.2f, 4.6f, 343.0f),
			Vertex(-50.3f, -200.0f, -443635.0f),
			Vertex(20000.0f, 4.5f, 0.0f)
		},
		{	// Invalid triangle (NAN)
			Vertex(1.0f, 0.4f, 1.2f),
			Vertex(0.4f, 0.5f, -1.2),
			Vertex(1.0f, NAN, 1.0f)
		},
		{	// Invalid triangle (INF)
			Vertex(std::numeric_limits<nr_float>::infinity(), 1.3f, -3435.0f),
			Vertex(0.4f, 100.2f, -1.2f),
			Vertex(0.4f, 0.5f, -1.2f)
		},
		{	// OK triangle (CW)
			Vertex(1.2f, 4.6f, 343.0f),
			Vertex(20000.0f, 4.5f, 0.0f),
			Vertex(-50.3f, -200.0f, -443635.0f)
		}
	};

	detail::TriangleRecord actual[count];

	auto code = Module(defaultContext, Module::Sources{ detail::clcode::base, detail::clcode::vertex_post_processor }, err);
	ASSERT_SUCCESS(err);
	ASSERT_SUCCESS(code.build(defaultDevice, Module::Options{ Module::CL_VERSION_12, Module::DEBUG }));

	auto testee = detail::VertexPostProcessor(code, err);
	ASSERT_SUCCESS(err);

	auto in = Buffer::make<Triangle>(defaultContext, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, count, triangles, err);
	ASSERT_SUCCESS(err);
	auto out = Buffer::make<detail::TriangleRecord>(defaultContext, CL_MEM_READ_WRITE, count, err);
	ASSERT_SUCCESS(err);

	ASSERT_SUCCESS(testee.setTriangleRecordInputBuffer(in));
	ASSERT_SUCCESS(testee.setTriangleRecordOutputBuffer(out));
	testee.setExecutionRange(count);
	
	ASSERT_SUCCESS(defaultCommandQueue.enqueueDispatchCommand(testee));
	ASSERT_SUCCESS(defaultCommandQueue.enqueueBufferReadCommand(out, false, count, actual));
	ASSERT_SUCCESS(defaultCommandQueue.await());
	
	EXPECT_PRED1(std::not_fn(std::isfinite<nr_float>), actual[0].triangle[0][0]);
	EXPECT_PRED1(std::not_fn(std::isfinite<nr_float>), actual[1].triangle[0][0]);
	EXPECT_PRED1(std::not_fn(std::isfinite<nr_float>), actual[2].triangle[0][0]);
	EXPECT_PRED1(std::not_fn(std::isfinite<nr_float>), actual[3].triangle[0][0]);

	EXPECT_EQ(actual[4].triangle, triangles[4]);
}
