#pragma once

#include <cmath>

#include <base/Module.h>
#include <base/Buffer.h>
#include <kernels/base.cl.h>
#include <kernels/vertex_reducer.cl.h>
#include <pipeline/VertexReducer.h>

#include "../../includes.h"

using namespace nr;
using namespace nr::detail;
using namespace testing;

TEST(VertexShader, Perspective)
{
    cl_status err = CL_SUCCESS;

    Vertex p;
    p.values[0] = 1;
    p.values[1] = 1;
    p.values[2] = 2;
	p.values[3] = nanf("");
    nr_float near[3] = { 0, 0, 0 };
    nr_float far[3]  = { 2, 2, 3 };
    Vertex result;

    Vertex expected;
    expected.values[0] = -0.5;
    expected.values[1] = -0.5;
    expected.values[2] = 2.0f / 3;
	expected.values[3] = nanf("");
    
    Module::Options options = 
    {
        Module::CL_VERSION_12, 
        Module::DEBUG
    };

    Module code(defaultContext, Module::Sources{clcode::base, clcode::vertex_reduce}, err);
    ASSERT_SUCCESS(err);

    ASSERT_SUCCESS(code.build(defaultDevice, options));

    auto testee = VertexReducer(code, err);
    ASSERT_SUCCESS(err);

    auto q = defaultCommandQueue;
    ASSERT_SUCCESS(err);
	
    auto d_point = Buffer::make<Vertex>(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 1, &p, err);
    ASSERT_SUCCESS(err);
    auto d_near = Buffer::make<nr_float>(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 3, near, err);
    ASSERT_SUCCESS(err);
    auto d_far = Buffer::make<nr_float>(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 3, far, err);
    ASSERT_SUCCESS(err);
    auto d_result = Buffer::make<Vertex>(defaultContext, CL_MEM_READ_WRITE, 1, err);
    ASSERT_SUCCESS(err);

	testee.setExecutionRange(1);

	ASSERT_SUCCESS(testee.setSimplexInputBuffer(d_point));
	ASSERT_SUCCESS(testee.setNearPlaneBuffer(d_near));
	ASSERT_SUCCESS(testee.setFarPlaneBuffer(d_far));
	ASSERT_SUCCESS(testee.setSimplexOutputBuffer(d_result));
    ASSERT_SUCCESS(q.enqueueDispatchCommand(testee));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_result, false, 4, result.values));
    ASSERT_SUCCESS(q.await());

    ASSERT_EQ(expected, result);
}