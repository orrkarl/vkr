#pragma once

#include <inc/includes.h>

#include <base/Module.h>
#include <kernels/base.cl.h>
#include <kernels/fine_rasterizer.cl.h>
#include <base/Buffer.h>
#include <rendering/Render.h>
#include "fine_utils.h"

using namespace nr;
using namespace nr::__internal;
using namespace testing;

struct PointInTriangleParams
{
    cl_int init(cl::CommandQueue queue) { return CL_SUCCESS; }
    
    cl_int load(cl::Kernel kernel)
    {
        cl_int err = CL_SUCCESS;
        NRuint argc = 0;

        if ((err = kernel.setArg(argc++, triangle.getBuffer())) != CL_SUCCESS) return err;
        if ((err = kernel.setArg(argc++, position)) != CL_SUCCESS) return err;
        if ((err = kernel.setArg(argc++, screenDim)) != CL_SUCCESS) return err;
        return kernel.setArg(argc, result.getBuffer());
    }

    Buffer triangle;
    ScreenPosition position;
    ScreenDimension screenDim;
    Buffer result; 
};

TEST(Fine, PointInTriangle)
{
    Error error = Error::NO_ERROR;
    cl_int err = CL_SUCCESS;

    const NRuint dim = 6;

    Triangle<dim> triangle;
    const NDCPosition left_bottom  = { -0.5, -0.5 };
    const NDCPosition right_bottom = { 0.5, -0.5 };
    const NDCPosition top          = { 0, 0.5 };
    mkTriangleInCoordinates(left_bottom, top, right_bottom, &triangle);

    const ScreenDimension screenDim  = { 1920, 1080 };
    const ScreenPosition origin      = { (NRuint) (0.5 * 1920), (NRuint) (0.5 * 1080) };
    const ScreenPosition belowBottom = { (NRuint) (0.5 * 1920), (NRuint) (0.1 * 1080) };
    const ScreenPosition justInside  = { (NRuint) (0.65 * 1920), (NRuint) (0.4 * 1080) };

    cl_bool h_result = CL_FALSE;

    auto q = cl::CommandQueue::getDefault();

    const char options_fmt[] = "-cl-std=CL2.0 -Werror -D _DEBUG -D _TEST_FINE -D RENDER_DIMENSION=%d";
    char options[sizeof(options_fmt) * 2];
    memset(options, 0, sizeof(options));
    sprintf(options, options_fmt, dim);

    Module code({clcode::base, clcode::fine_rasterizer}, options, &err);
    ASSERT_EQ(CL_SUCCESS, err);

    auto testee = code.makeKernel<PointInTriangleParams>("is_point_in_triangle_test", &err);
    ASSERT_EQ(CL_SUCCESS, err);

    Buffer d_triangle(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(triangle), &triangle, &error);
    ASSERT_PRED1(error::isSuccess, error);

    Buffer d_result(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_bool), &h_result, &error);
    ASSERT_PRED1(error::isSuccess, error);

    testee.params.triangle = d_triangle;
    testee.params.screenDim = screenDim;
    testee.params.result = d_result;

    testee.global = cl::NDRange(1);
    testee.local  = cl::NDRange(1);

    testee.params.position = origin;
    ASSERT_EQ(CL_SUCCESS, testee(q));
    ASSERT_EQ(CL_SUCCESS, q.enqueueReadBuffer(d_result.getBuffer(), CL_FALSE, 0, sizeof(h_result), &h_result));
    ASSERT_EQ(CL_SUCCESS, q.finish());
    ASSERT_EQ(CL_TRUE, h_result);

    testee.params.position = belowBottom;
    ASSERT_EQ(CL_SUCCESS, testee(q));
    ASSERT_EQ(CL_SUCCESS, q.enqueueReadBuffer(d_result.getBuffer(), CL_FALSE, 0, sizeof(h_result), &h_result));
    ASSERT_EQ(CL_SUCCESS, q.finish());
    ASSERT_EQ(CL_FALSE, h_result);

    testee.params.position = justInside;
    ASSERT_EQ(CL_SUCCESS, testee(q));
    ASSERT_EQ(CL_SUCCESS, q.enqueueReadBuffer(d_result.getBuffer(), CL_FALSE, 0, sizeof(h_result), &h_result));
    ASSERT_EQ(CL_SUCCESS, q.finish());
    ASSERT_EQ(CL_TRUE, h_result);
}
