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

void validateFragment(const Fragment& fragment, const ScreenDimension& screenDim, const RawColorRGBA* color, const nr_float* depth)
{
    const nr_uint idx = index_from_screen(fragment.position, screenDim);

    ASSERT_EQ(fragment.color, color[idx]);
    ASSERT_EQ(fragment.depth, depth[idx]);
}

AssertionResult validateDepth(const nr_float* depthBuffer, const ScreenDimension& screenDim, const nr_float defaultDepth, const nr_float expectedDepth)
{
    for (nr_uint y = 0; y < screenDim.height; ++y)
    {
        for (nr_uint x = 0; x < screenDim.width; ++x)
        {
            nr_float actualDepth = depthBuffer[y * screenDim.width + x];
            nr_float expected = 1 / expectedDepth;
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

    const nr_uint dim = 5;
	const nr_uint point_count = dim + 1;

    const ScreenDimension screenDim = { 1366, 768 };
    const nr_uint totalScreenSize = screenDim.width * screenDim.height;

    const BinQueueConfig config = { 32, 32, 256 };
    const nr_uint binCountX = ceil(((nr_float) screenDim.width) / config.binWidth);
    const nr_uint binCountY = ceil(((nr_float) screenDim.height) / config.binHeight);
    const nr_uint totalBinCount = binCountX * binCountY;
    const nr_uint totalWorkGroupCount = 6;
    const nr_uint totalBinQueuesSize = totalWorkGroupCount * totalBinCount * (config.queueSize + 1); 

    const nr_float defaultDepth = 1;
    const nr_float expectedDepth = 0.5;

    const nr_uint triangleCount = 3 * totalBinCount;
    std::unique_ptr<Triangle<dim>[]> h_triangles(new Triangle<dim>[triangleCount]);
    std::unique_ptr<nr_uint[]> h_binQueues(new nr_uint[totalBinQueuesSize]);

    std::unique_ptr<RawColorRGBA[]> h_colorBuffer(new RawColorRGBA[totalScreenSize]);
    std::unique_ptr<nr_float[]> h_depthBuffer(new nr_float[totalScreenSize]);

    Fragment expected;
    expected.color = RED;

    for (nr_uint i = 0; i < totalScreenSize; ++i)
    {
        h_colorBuffer[i] = { 0, 0, 0 };
        h_depthBuffer[i] = defaultDepth;
    }
    
    fillTriangles<dim>(screenDim, config, totalWorkGroupCount, expectedDepth, 256, h_triangles.get(), h_binQueues.get());

    auto code = mkFineModule(dim, &err);
    ASSERT_SUCCESS(err);

    auto testee = FineRasterizer(code, &err);
    ASSERT_SUCCESS(err);

    auto q = defaultCommandQueue;

    FrameBuffer frame;
    frame.color = Buffer<nr::RawColorRGBA>(defaultContext, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, totalScreenSize, h_colorBuffer.get(), &err);
    ASSERT_SUCCESS(err);
    frame.depth = Buffer<nr_float>(defaultContext, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, totalScreenSize, h_depthBuffer.get(), &err);
    ASSERT_SUCCESS(err);

    Buffer<nr_float> d_triangles(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, triangleCount * sizeof(Triangle<dim>) / sizeof(nr_float), (nr_float*) h_triangles.get(), &err);
    ASSERT_SUCCESS(err);
    Buffer<nr_uint> d_binQueues(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, totalBinQueuesSize, h_binQueues.get(), &err);
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
