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
    cl_int err = CL_SUCCESS;
    Error error = Error::NO_ERROR;

    const NRuint dim = 3;

    Point<dim> p;
    p.values[0] = 1;
    p.values[1] = 1;
    p.values[2] = 2;
    NRfloat near[dim] = { 0, 0, 0 };
    NRfloat far[dim]  = { 2, 2, 3 };
    Point<dim> result;

    Point<dim> expected;
    expected.values[0] = -0.5;
    expected.values[1] = -0.5;
    expected.values[2] = 1.0 / 3;
    
    const char options_fmt[] = "-cl-std=CL2.0 -Werror -D _DEBUG -D _TEST_FINE -D RENDER_DIMENSION=%d";
    char options[sizeof(options_fmt) * 2];
    memset(options, 0, sizeof(options));
    sprintf(options, options_fmt, dim);

    Module code({clcode::base, clcode::vertex_shading}, options, &err);
    ASSERT_TRUE(isSuccess(err));

    VertexShader testee = code.makeKernel<VertexShadingParams>("shade_vertex", &err);
    ASSERT_TRUE(isSuccess(err));

    auto q = cl::CommandQueue::getDefault(&err);
    ASSERT_TRUE(isSuccess(err));

    Buffer d_point(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(p.values), p.values, &error);
    ASSERT_TRUE(isSuccess(err));
    Buffer d_near(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(near), near, &error);
    ASSERT_TRUE(isSuccess(err));
    Buffer d_far(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(far), far, &error);
    ASSERT_TRUE(isSuccess(err));
    Buffer d_result(CL_MEM_READ_WRITE, sizeof(result.values), &error);
    ASSERT_TRUE(isSuccess(err));

    testee.params.points = d_point;
    testee.params.near   = d_near;
    testee.params.far    = d_far;
    testee.params.result = d_result;

    testee.global = cl::NDRange(1);
    testee.local  = cl::NDRange(1);

    ASSERT_TRUE(isSuccess(testee(q)));
    ASSERT_TRUE(isSuccess(q.enqueueReadBuffer(d_result, CL_FALSE, 0, sizeof(result.values), result.values)));
    ASSERT_TRUE(isSuccess(q.finish()));

    ASSERT_EQ(expected, result);
}