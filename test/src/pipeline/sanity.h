#pragma once

#include "../includes.h"

#include <pipeline/Pipeline.h>
#include <pipeline/VertexBuffer.h>

#include <utils/rendermath.h>

#include "utils.h"

nr_float h_near[]
{
	-5, -5, 0.5
};

nr_float h_far[]
{
	5, 5, 10
};

TEST(Pipeline, Sanity)
{
	cl_status ret = CL_SUCCESS;

	constexpr nr::ScreenDimension screenDim = { 640, 480 };
	constexpr nr_uint workGroupCount = 7;
	constexpr nr_uint batchSize = 4;
	constexpr nr::detail::BinQueueConfig config = { 64, 64, (batchSize + 1) * workGroupCount };

	auto bins = nr::detail::getBinCount(screenDim, config);
	const nr_uint count = workGroupCount * bins.first * bins.second * batchSize;

	auto bitmap = std::make_unique<nr::RawColorRGBA[]>(screenDim.getTotalSize());
	
	auto triangles = std::make_unique<nr::Triangle[]>(count);
	fillTriangleBuffer(screenDim, config, workGroupCount, batchSize, 0.5f, triangles.get());

	auto vb = nr::VertexBuffer::make(defaultContext, count, triangles.get(), ret);
	ASSERT_SUCCESS(ret);

	auto p = nr::Pipeline(defaultContext, defaultDevice, defaultCommandQueue, config, workGroupCount, batchSize, ret);
	ASSERT_SUCCESS(ret);

	ret = p.viewport(screenDim);
	ASSERT_SUCCESS(ret);

	p.setClearColor({ 0, 0, 0, 0 });
	p.setClearDepth(1.0f);

	ret = p.setNearPlane(h_near);
	ASSERT_SUCCESS(ret);

	ret = p.setFarPlane(h_far);
	ASSERT_SUCCESS(ret);

	ret = p.clear();
	ASSERT_SUCCESS(ret);

	ret = p.render(vb, nr::Primitive::TRIANGLE, count);
	ASSERT_SUCCESS(ret);

	ret = p.copyFrameBuffer(bitmap.get());
	ASSERT_SUCCESS(ret);
}