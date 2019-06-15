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

struct ShadeTestParams
{
    cl_int init(cl::CommandQueue queue) { return CL_SUCCESS; }

    cl_int load(cl::Kernel kernel)
    {
        cl_int err = CL_SUCCESS;
        NRuint argc = 0;

        if ((err = kernel.setArg(argc++, fragment)) != CL_SUCCESS) return err;
        if ((err = kernel.setArg(argc++, screenDim)) != CL_SUCCESS) return err;
        if ((err = kernel.setArg(argc++, frame.color.getBuffer())) != CL_SUCCESS) return err;
        return kernel.setArg(argc++, frame.depth.getBuffer());
    }

    Fragment fragment;
    ScreenDimension screenDim;
    FrameBuffer frame;
};

TEST(Fine, ShadeTest)
{
    cl_int err = CL_SUCCESS;

    const NRuint dim = 6;
    constexpr ScreenDimension screenDim = { 5, 2 };

    const NRchar options_fmt[] = "-cl-std=CL2.0 -Werror -D _DEBUG -D _TEST_FINE -D RENDER_DIMENSION=%d";
    NRchar options[sizeof(options_fmt) * 2];
    memset(options, 0, sizeof(options));
    sprintf(options, options_fmt, dim);

    RawColorRGB h_color[screenDim.width * screenDim.height];
    Depth       h_depth[screenDim.width * screenDim.height];

    for (NRuint i = 0; i < screenDim.width * screenDim.height; ++i)
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
    secondFrag.depth = firstFrag.depth / 1.1;

    const NRuint idx = index_from_screen(firstFrag.position, screenDim);

    FrameBuffer frame;
    frame.color = Buffer(CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(h_color), h_color, &err);
    ASSERT_PRED1(error::isSuccess, err);
    frame.depth = Buffer(CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(h_depth), h_depth, &err);
    ASSERT_PRED1(error::isSuccess, err);
    
    Module code({clcode::base, clcode::fine_rasterizer}, options, &err);
    ASSERT_TRUE(isSuccess(err));

    auto testee = code.makeKernel<ShadeTestParams>("shade_test", &err);
    ASSERT_TRUE(isSuccess(err));

    auto q = cl::CommandQueue::getDefault(&err);
    ASSERT_TRUE(isSuccess(err));    

    testee.params.fragment = firstFrag;
    testee.params.screenDim = screenDim;
    testee.params.frame = frame;

    testee.global = cl::NDRange(1);
    testee.local  = cl::NDRange(1);

    ASSERT_TRUE(isSuccess(testee(q)));
    ASSERT_TRUE(isSuccess(q.enqueueReadBuffer(frame.color, CL_FALSE, 0, sizeof(h_color), h_color)));
    ASSERT_TRUE(isSuccess(q.enqueueReadBuffer(frame.depth, CL_FALSE, 0, sizeof(h_depth), h_depth)));
    ASSERT_TRUE(isSuccess(q.finish()));

    ASSERT_EQ(firstFrag.color, h_color[idx]);
    ASSERT_EQ(firstFrag.depth, h_depth[idx]);

    testee.params.fragment = secondFrag;
    ASSERT_TRUE(isSuccess(testee(q)));
    ASSERT_TRUE(isSuccess(q.enqueueReadBuffer(frame.color, CL_FALSE, 0, sizeof(h_color), h_color)));
    ASSERT_TRUE(isSuccess(q.enqueueReadBuffer(frame.depth, CL_FALSE, 0, sizeof(h_depth), h_depth)));
    ASSERT_TRUE(isSuccess(q.finish()));

    ASSERT_EQ(firstFrag.color, h_color[idx]);
    ASSERT_EQ(firstFrag.depth, h_depth[idx]);
}
