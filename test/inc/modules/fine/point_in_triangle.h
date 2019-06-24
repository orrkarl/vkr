#pragma once

#include <inc/includes.h>

#include <base/Buffer.h>
#include <base/Module.h>

#include <kernels/base.cl.h>
#include <kernels/fine_rasterizer.cl.h>

#include <rendering/Render.h>

#include "fine_utils.h"

using namespace nr;
using namespace nr::__internal;
using namespace testing;

struct PointInTriangle : Kernel
{    
    PointInTriangle(Module code, cl_status* err)
        : Kernel(code, "is_point_in_triangle_test", err)
    {
    }

    cl_status load()
    {
        cl_status err = CL_SUCCESS;
        NRuint argc = 0;

        if ((err = setArg(argc++, triangle)) != CL_SUCCESS) return err;
        if ((err = setArg(argc++, position)) != CL_SUCCESS) return err;
        if ((err = setArg(argc++, screenDim)) != CL_SUCCESS) return err;
        return setArg(argc, result);
    }

    Buffer<NRfloat> triangle;
    ScreenPosition position;
    ScreenDimension screenDim;
    Buffer<NRbool> result; 
};

TEST(Fine, PointInTriangle)
{
    cl_status err = CL_SUCCESS;

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

    NRbool h_result = false;

    auto q = CommandQueue::getDefault();

    auto code = mkFineModule(dim, &err);
    ASSERT_SUCCESS(err);

    auto testee = PointInTriangle(code, &err);
    ASSERT_SUCCESS(err);

    Buffer<NRfloat> d_triangle(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(triangle) / sizeof(NRfloat), (NRfloat*) &triangle, &err);
    ASSERT_SUCCESS(err);

    Buffer<NRbool> d_result(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 1, &h_result, &err);
    ASSERT_SUCCESS(err);

    testee.triangle = d_triangle;
    testee.screenDim = screenDim;
    testee.result = d_result;

    std::array<size_t, 1> global = {1};
    std::array<size_t, 1> local  = {1};

    testee.position = origin;
    ASSERT_SUCCESS(testee.load());
    ASSERT_SUCCESS(q.enqueueKernelCommand<1>(testee, global, local));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_result, false, 1, &h_result));
    ASSERT_SUCCESS(q.await());
    ASSERT_EQ(true, h_result);

    testee.position = belowBottom;
    ASSERT_SUCCESS(testee.load());
    ASSERT_SUCCESS(q.enqueueKernelCommand<1>(testee, global, local));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_result, false, 1, &h_result));
    ASSERT_SUCCESS(q.await());
    ASSERT_EQ(false, h_result);

    testee.position = justInside;
    ASSERT_SUCCESS(testee.load());
    ASSERT_SUCCESS(q.enqueueKernelCommand<1>(testee, global, local));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_result, false, 1, &h_result));
    ASSERT_SUCCESS(q.await());
    ASSERT_EQ(true, h_result);
}
