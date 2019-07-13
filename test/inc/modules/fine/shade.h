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

struct ShadeTest : Kernel
{
    ShadeTest(Module code, cl_status* err)
        : Kernel(code, "shade_test", err)
    {
    }

    cl_status load()
    {
        cl_status err = CL_SUCCESS;
        nr_uint argc = 0;

        if ((err = setArg(argc++, fragment)) != CL_SUCCESS) return err;
        if ((err = setArg(argc++, screenDim)) != CL_SUCCESS) return err;
        if ((err = setArg(argc++, frame.color)) != CL_SUCCESS) return err;
        return setArg(argc++, frame.depth);
    }

    Fragment fragment;
    ScreenDimension screenDim;
    FrameBuffer frame;
};

TEST(Fine, ShadeTest)
{
    cl_status err = CL_SUCCESS;

    const nr_uint dim = 6;
    constexpr ScreenDimension screenDim = { 5, 2 };
    constexpr nr_uint totalScreenSize = screenDim.width * screenDim.height;

    RawColorRGBA h_color[totalScreenSize];
    Depth       h_depth[totalScreenSize];

    for (nr_uint i = 0; i < totalScreenSize; ++i)
    {
        h_color[i] = {0, 0, 0};
        h_depth[i] = 1;
    }

    Fragment firstFrag;
    firstFrag.position = { screenDim.width / 5, screenDim.height / 2 };
    firstFrag.color = { 255, 0, 0 };
    firstFrag.depth = 1 / 0.5;

    Fragment secondFrag;
    secondFrag.position = firstFrag.position;
    secondFrag.color = { 255, 0, 0 };
    secondFrag.depth = firstFrag.depth / 1.1f;

    const nr_uint idx = index_from_screen(firstFrag.position, screenDim);

    FrameBuffer frame;
    frame.color = Buffer<RawColorRGBA>(CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, totalScreenSize, h_color, &err);
    ASSERT_SUCCESS(err);
    frame.depth = Buffer<nr_float>(CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, totalScreenSize, h_depth, &err);
    ASSERT_SUCCESS(err);
    
    auto code = mkFineModule(dim, &err);
    ASSERT_SUCCESS(err);

    auto testee = ShadeTest(code, &err);
    ASSERT_SUCCESS(err);

    auto q = CommandQueue::getDefault();
    ASSERT_SUCCESS(err);

    testee.fragment = firstFrag;
    testee.screenDim = screenDim;
    testee.frame = frame;

    std::array<size_t, 1> global = { 1 };
    std::array<size_t, 1> local  = { 1 };

    ASSERT_SUCCESS(testee.load());
    ASSERT_SUCCESS(q.enqueueKernelCommand<1>(testee, global, local));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(frame.color, false, totalScreenSize, h_color));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(frame.depth, false, totalScreenSize, h_depth));
    ASSERT_SUCCESS(q.await());

    ASSERT_EQ(firstFrag.color, h_color[idx]);
    ASSERT_EQ(firstFrag.depth, h_depth[idx]);

    testee.fragment = secondFrag;
    ASSERT_SUCCESS(testee.load());
    ASSERT_SUCCESS(q.enqueueKernelCommand<1>(testee, global, local));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(frame.color, false, totalScreenSize, h_color));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(frame.depth, false, totalScreenSize, h_depth));
    ASSERT_SUCCESS(q.await());

    ASSERT_EQ(firstFrag.color, h_color[idx]);
    ASSERT_EQ(firstFrag.depth, h_depth[idx]);
}
