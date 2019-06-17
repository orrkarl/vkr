#pragma once

#include <inc/includes.h>

#include <base/Module.h>
#include <kernels/base.cl.h>
#include <kernels/fine_rasterizer.cl.h>
#include <base/Buffer.h>
#include <rendering/Render.h>
#include <pipeline/BinRasterizer.h>
#include <pipeline/FineRasterizer.h>
#include "fine_utils.h"

using namespace nr;
using namespace nr::__internal;
using namespace testing;

void validateFragment(const Fragment& fragment, const ScreenDimension& screenDim, const RawColorRGB* color, const NRfloat* depth)
{
    const NRuint idx = index_from_screen(fragment.position, screenDim);

    ASSERT_EQ(fragment.color, color[idx]);
    ASSERT_EQ(fragment.depth, depth[idx]);
}

AssertionResult validateDepth(const NRfloat* depthBuffer, const ScreenDimension& screenDim, const NRfloat defaultDepth, const NRfloat expectedDepth)
{
    for (NRuint y = 0; y < screenDim.height; ++y)
    {
        for (NRuint x = 0; x < screenDim.width; ++x)
        {
            NRfloat actualDepth = depthBuffer[y * screenDim.width + x];
            NRfloat expected = 1 / expectedDepth;
            if (std::abs(actualDepth - defaultDepth) > 10e-5 && std::abs(actualDepth - expected) > 10e-5)
            {
                return AssertionFailure() << "At: (" << x << ", " << y << "). default = " << defaultDepth << ", expected = " << expected << ", actual = " << actualDepth;
            }
        }
    }

    return AssertionSuccess();
}

TEST(Fine, Rasterizer)
{
    cl_int err = CL_SUCCESS;

    const NRuint dim = 5;

    const ScreenDimension screenDim = { 1366, 768 };
    const NRuint totalScreenSize = screenDim.width * screenDim.height;

    const BinQueueConfig config = { 32, 32, 256 };
    const NRuint binCountX = ceil(((NRfloat) screenDim.width) / config.binWidth);
    const NRuint binCountY = ceil(((NRfloat) screenDim.height) / config.binHeight);
    const NRuint totalBinCount = binCountX * binCountY;
    const NRuint totalWorkGroupCount = 4;
    const NRuint totalBinQueuesSize = totalWorkGroupCount * totalBinCount * (config.queueSize + 1); 

    const NRfloat defaultDepth = 1;
    const NRfloat expectedDepth = 0.5;

    const NRuint triangleCount = 3 * totalBinCount;
    std::unique_ptr<Triangle<dim>[]> h_triangles(new Triangle<dim>[triangleCount]);
    std::unique_ptr<NRuint[]> h_binQueues(new NRuint[totalBinQueuesSize]);

    std::unique_ptr<RawColorRGB[]> h_colorBuffer(new RawColorRGB[totalScreenSize]);
    std::unique_ptr<NRfloat[]> h_depthBuffer(new NRfloat[totalScreenSize]);

    Fragment expected;
    expected.color = RED;

    for (NRuint i = 0; i < totalScreenSize; ++i)
    {
        h_colorBuffer[i] = { 0, 0, 0 };
        h_depthBuffer[i] = defaultDepth;
    }
    
    fillTriangles<dim>(screenDim, config, totalWorkGroupCount, expectedDepth, 256, h_triangles.get(), h_binQueues.get());

    auto code = mkFineModule(dim, &err);
    ASSERT_TRUE(isSuccess(err));

    auto testee = code.makeKernel<FineRasterizerParams>("fine_rasterize", &err);
    ASSERT_TRUE(isSuccess(err));

    auto q = cl::CommandQueue::getDefault(&err);
    ASSERT_TRUE(isSuccess(err));

    FrameBuffer frame;
    frame.color = Buffer(CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, totalScreenSize * sizeof(RawColorRGB), h_colorBuffer.get(), &err);
    ASSERT_TRUE(isSuccess(err));
    frame.depth = Buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, totalScreenSize * sizeof(NRfloat), h_depthBuffer.get(), &err);
    ASSERT_TRUE(isSuccess(err));

    Buffer d_triangles(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, triangleCount * sizeof(Triangle<dim>), h_triangles.get(), &err);
    ASSERT_TRUE(isSuccess(err));
    Buffer d_binQueues(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, totalBinQueuesSize * sizeof(NRuint), h_binQueues.get(), &err);
    ASSERT_TRUE(isSuccess(err));

    testee.params.triangleData = d_triangles;
    testee.params.dim = screenDim;
    testee.params.binQueueConfig = config;
    testee.params.binQueues = d_binQueues;
    testee.params.workGroupCount = totalWorkGroupCount;
    testee.params.frameBuffer = frame;

    testee.global = cl::NDRange(binCountX, binCountY);
    testee.local  = cl::NDRange(binCountX, binCountY / totalWorkGroupCount);

    ASSERT_TRUE(isSuccess(testee(q)));
    ASSERT_TRUE(isSuccess(q.enqueueReadBuffer(frame.color, CL_FALSE, 0, totalScreenSize * sizeof(RawColorRGB), h_colorBuffer.get())));
    ASSERT_TRUE(isSuccess(q.enqueueReadBuffer(frame.depth, CL_FALSE, 0, totalScreenSize * sizeof(NRfloat), h_depthBuffer.get())));
    ASSERT_TRUE(isSuccess(q.finish()));
    ASSERT_TRUE(validateDepth(h_depthBuffer.get(), screenDim, defaultDepth, expectedDepth));
}
