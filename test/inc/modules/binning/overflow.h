#pragma once

#include <inc/includes.h>

#include "bin_utils.h"

#include <base/Module.h>
#include <pipeline/BinRasterizer.h>
#include <base/Buffer.h>

#include <chrono>
#include <memory>

using namespace nr;
using namespace nr::__internal;
using namespace testing;

TEST(Binning, RasterizerOverflow)
{
    cl_status err = CL_SUCCESS; 

    const nr_uint dim = 4;
    const nr_uint triangleCount = 400;

    const nr_uint workGroupCount = 2;
    const ScreenDimension screenDim = {4, 10};
    const BinQueueConfig config = {32, 32, 10};

    nr_bool isOverflowing = false;

    auto q = defaultCommandQueue;

	std::unique_ptr<Triangle<dim>[]> h_triangles_(new Triangle<dim>[triangleCount]);
	auto h_triangles = h_triangles_.get();
    for (nr_uint i = 0; i < triangleCount; ++i)
    {
        mkTriangleInCoords(0, 0, screenDim, h_triangles + i);
    }
    
    auto code = mkBinningModule(dim, triangleCount, &err);
    ASSERT_SUCCESS(err);

    auto testee = BinRasterizer(code, &err);
    ASSERT_SUCCESS(err);
    
    Buffer<nr_float> d_triangles(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, triangleCount * sizeof(Triangle<dim>) / sizeof(nr_float), (nr_float*) h_triangles, &err);
    ASSERT_SUCCESS(err);

    Buffer<nr_bool> d_overflow(defaultContext, CL_MEM_READ_WRITE, 1, &err);
    ASSERT_SUCCESS(err);

    Buffer<nr_uint> d_binQueues(defaultContext, CL_MEM_READ_WRITE, BinRasterizer::getTotalBinQueueCount(workGroupCount, screenDim, config), &err);
    ASSERT_SUCCESS(err);

    Buffer<nr_uint> d_batchIndex(defaultContext, CL_MEM_READ_WRITE, 1, &err);
    ASSERT_SUCCESS(err);

    testee.binQueueConfig = config;
    testee.dimension = screenDim;
    testee.triangleData = d_triangles;
    testee.triangleCount = triangleCount;
    testee.hasOverflow = d_overflow;
    testee.batchIndex = d_batchIndex;

    std::array<size_t, 2> global = { workGroupCount * screenDim.width, screenDim.height };
    std::array<size_t, 2> local  = { screenDim.width, screenDim.height };

    ASSERT_SUCCESS(testee.load());
    ASSERT_SUCCESS(q.enqueueKernelCommand<2>(testee, global, local));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_overflow, false, 1, &isOverflowing));
    ASSERT_SUCCESS(q.await());
    ASSERT_TRUE(isOverflowing);
}
