#pragma once

#include <base/Module.h>
#include <base/Buffer.h>
#include <kernels/base.cl.h>
#include <kernels/vertex_post_processor.cl.h>
#include <rendering/Render.h>
#include <utils/StandardDispatch.h>

#include "../../../includes.h"

using namespace nr;
using namespace testing;

class AreaKernel : public StandardDispatch<1, Triangle, Buffer>
{
public:
	AreaKernel(const Module& module, cl_status& err)
		: StandardDispatch<1, Triangle, Buffer>(module, "test_area", err)
	{
		range.global.x = 1;
		range.local.x = 1;
	}

	cl_status setTriangle(const Triangle& triangle)
	{
		return setArg<0>(triangle);
	}

	cl_status setResultBuffer(const Buffer& result)
	{
		return setArg<1>(result);
	}
};	

void testArea(const Triangle& tri, const nr_float expectedArea)
{
	cl_status err = CL_SUCCESS;

	nr_float actualArea;

	auto code = Module(defaultContext, Module::Sources{ detail::clcode::base, detail::clcode::vertex_post_processor }, err);
	ASSERT_SUCCESS(err);

	ASSERT_SUCCESS(code.build(defaultDevice, Module::Options{ Module::CL_VERSION_12, Module::DEBUG }));

	Buffer result = Buffer::make<nr_float>(defaultContext, CL_MEM_READ_WRITE, 1, err);
	ASSERT_SUCCESS(err);

	auto testee = AreaKernel(code, err);
	ASSERT_SUCCESS(err);

	ASSERT_SUCCESS(testee.setResultBuffer(result));
	ASSERT_SUCCESS(testee.setTriangle(tri));
	ASSERT_SUCCESS(defaultCommandQueue.enqueueDispatchCommand(testee));
	ASSERT_SUCCESS(defaultCommandQueue.enqueueBufferReadCommand(result, false, &actualArea));
	ASSERT_SUCCESS(defaultCommandQueue.await());

	ASSERT_NEAR(actualArea, expectedArea, 1e-7);
}

TEST(VertexPostProcessing, CCWArea)
{
	const nr_float length = 1.5f, height = 0.63f;
	const nr_float expectedArea = std::abs(length * height / 2);
	Triangle ccw =
	{
		Vertex(0.0f, 0.0f, 0.0f),
		Vertex(length, 0.0f, 0.0f),
		Vertex(0.0f, height, 0.0f)
	};

	testArea(ccw, expectedArea);
}

TEST(VertexPostProcessing, CWArea)
{
	const nr_float length = -0.5f, height = -50.94f, originHeight = 0.3f;
	const nr_float expectedArea = -std::abs((originHeight - height) * length / 2);
	Triangle cw =
	{
		Vertex(0.0f, originHeight, 1.0f),
		Vertex(0.0f, height, -453454.55f),
		Vertex(length, 0.0f, 254.118f)
	};

	testArea(cw, expectedArea);
}

TEST(VertexPostProcessing, DegenerateArea)
{
	const nr_float length = 1.5f, height = 0.63f;
	Triangle simple =
	{
		Vertex(0.0f, 0.0f, 0.0f),
		Vertex(length, height, 0.0f),
		Vertex(2.556f * length, 2.556f * height, 0.0f)
	};

	testArea(simple, 0);
}