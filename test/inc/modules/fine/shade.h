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

        if ((err = kernel.setArg(argc++, triangles.getBuffer())) != CL_SUCCESS) return err;
        if ((err = kernel.setArg(argc++, triangleIndex)) != CL_SUCCESS) return err;
        if ((err = kernel.setArg(argc++, fragment)) != CL_SUCCESS) return err;
        if ((err = kernel.setArg(argc++, screenDim)) != CL_SUCCESS) return err;
        if ((err = kernel.setArg(argc++, color.getBuffer())) != CL_SUCCESS) return err;
        if ((err = kernel.setArg(argc++, depth.getBuffer())) != CL_SUCCESS) return err;
        return kernel.setArg(argc, stencil.getBuffer());
    }

    Buffer triangles;
    NRuint triangleIndex;
    Fragment fragment;
    ScreenDimension screenDim;
    Buffer color;
    Buffer depth;
    Buffer stencil;
};

TEST(Fine, ShadeTest)
{
    Error error = Error::NO_ERROR;
    cl_int err = CL_SUCCESS;

    const NRuint dim = 6;
    const ScreenDimension screenDim = { 20, 50 };

    const NRchar options_fmt[] = "-cl-std=CL2.0 -Werror -D _DEBUG -D _TEST_FINE -D RENDER_DIMENSION=%d";
    NRchar options[sizeof(options_fmt) * 2];
    memset(options, 0, sizeof(options));

    sprintf(options, options_fmt, dim);

    Module code({clcode::base, clcode::fine_rasterizer}, options, &err);
    ASSERT_EQ(CL_SUCCESS, err);

    auto testee = code.makeKernel<ShadeTestParams>("shade_test", &err);
    ASSERT_EQ(CL_SUCCESS, err);

    RawColorRGB h_color[screenDim.width * screenDim.height];
    Depth       h_depth[screenDim.width * screenDim.height];
    Index       h_stencil[screenDim.width * screenDim.height];

    memset(h_color, screenDim.width * screenDim.height, 0);
    memset(h_depth, screenDim.width * screenDim.height, 0);
    memset(h_stencil, screenDim.width * screenDim.height, 0);
    
    Buffer d_color(CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(h_color), h_color, &error);
    ASSERT_PRED1(error::isSuccess, error);
    Buffer d_depth(CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(h_depth), h_depth, &error);
    ASSERT_PRED1(error::isSuccess, error);
    Buffer d_stencil(CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(h_stencil), h_stencil, &error);
    ASSERT_PRED1(error::isSuccess, error);

    Fragment frag;
    frag.position = { 10, 10 };
    frag.color = { 255, 0, 0 };
    frag.depth = 0.5;
    frag.stencil = 0;

    Triangle<dim> triangle;    
    NDCPosition top = { 0, 0.5 };
    NDCPosition bottom_left = { -0.5, -0.5 };
    NDCPosition bottom_right = { 0.5, -0.5 };
    mkTriangleInCoordinates(top, bottom_left, bottom_right, &triangles);
}
