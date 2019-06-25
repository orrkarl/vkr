#pragma once

#include <inc/includes.h>

#include <base/Module.h>
#include <base/Buffer.h>
#include <kernels/base.cl.h>
#include <kernels/vertex_shading.cl.h>
#include <pipeline/VertexShader.h>

using namespace nr;
using namespace nr::__internal;
using namespace testing;

TEST(VertexShader, Perspective)
{
    cl_status err = CL_SUCCESS;

    const nr_uint dim = 3;

    Point<dim> p;
    p.values[0] = 1;
    p.values[1] = 1;
    p.values[2] = 2;
    nr_float near[dim] = { 0, 0, 0 };
    nr_float far[dim]  = { 2, 2, 3 };
    Point<dim> result;

    Point<dim> expected;
    expected.values[0] = -0.5;
    expected.values[1] = -0.5;
    expected.values[2] = 2.0f / 3;
    
    Module::Options options = 
    {
        Module::CL_VERSION_12, 
        Module::RenderDimension(dim), 
        Module::DEBUG
    };

    Module code({clcode::base, clcode::vertex_shading}, &err);
    ASSERT_SUCCESS(err);

    ASSERT_SUCCESS(code.build(options));

    auto testee = VertexShader(code, &err);
    ASSERT_SUCCESS(err);

    auto q = CommandQueue::getDefault();
    ASSERT_SUCCESS(err);

    Buffer<nr_float> d_point(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, dim, p.values, &err);
    ASSERT_SUCCESS(err);
    Buffer<nr_float> d_near(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, dim, near, &err);
    ASSERT_SUCCESS(err);
    Buffer<nr_float> d_far(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, dim, far, &err);
    ASSERT_SUCCESS(err);
    Buffer<nr_float> d_result(CL_MEM_READ_WRITE, dim, &err);
    ASSERT_SUCCESS(err);

    testee.points = d_point;
    testee.near   = d_near;
    testee.far    = d_far;
    testee.result = d_result;

    std::array<size_t, 1> global{1};
    std::array<size_t, 1> local{1};

    ASSERT_SUCCESS(testee.load());
    ASSERT_SUCCESS(q.enqueueKernelCommand<1>(testee, global, local));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_result, false, dim, result.values));
    ASSERT_SUCCESS(q.await());

    ASSERT_EQ(expected, result);
}