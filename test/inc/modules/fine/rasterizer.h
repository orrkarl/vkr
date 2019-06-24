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
    cl_status err = CL_SUCCESS;

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
    ASSERT_SUCCESS(err);

    auto testee = FineRasterizer(code, &err);
    ASSERT_SUCCESS(err);

    auto q = CommandQueue::getDefault();

    FrameBuffer frame;
    frame.color = Buffer<nr::RawColorRGB>(CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, totalScreenSize, h_colorBuffer.get(), &err);
    ASSERT_SUCCESS(err);
    frame.depth = Buffer<NRfloat>(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, totalScreenSize, h_depthBuffer.get(), &err);
    ASSERT_SUCCESS(err);

    Buffer<NRfloat> d_triangles(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, triangleCount * sizeof(Triangle<dim>) / sizeof(NRfloat), (NRfloat*) h_triangles.get(), &err);
    ASSERT_SUCCESS(err);
    Buffer<NRuint> d_binQueues(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, totalBinQueuesSize, h_binQueues.get(), &err);
    ASSERT_SUCCESS(err);

    testee.triangleData = d_triangles;
    testee.dim = screenDim;
    testee.binQueueConfig = config;
    testee.binQueues = d_binQueues;
    testee.workGroupCount = totalWorkGroupCount;
    testee.frameBuffer = frame;

    std::array<size_t, 2> global = { binCountX, binCountY };
    std::array<size_t, 2> local  = { binCountX, binCountY / totalWorkGroupCount };

    ASSERT_SUCCESS(testee.load());
    ASSERT_SUCCESS(q.enqueueKernelCommand<2>(testee, global, local));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(frame.color, false, totalScreenSize, h_colorBuffer.get()));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(frame.depth, false, totalScreenSize, h_depthBuffer.get()));
    ASSERT_SUCCESS(q.await());
    ASSERT_TRUE(validateDepth(h_depthBuffer.get(), screenDim, defaultDepth, expectedDepth));
}
