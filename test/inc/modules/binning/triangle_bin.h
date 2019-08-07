#pragma once

#include <inc/includes.h>

#include "bin_utils.h"

#include <base/Module.h>
#include <base/Buffer.h>
#include <rendering/Render.h>

using namespace nr;
using namespace nr::detail;
using namespace testing;

struct TriangleInBin : Kernel
{
    TriangleInBin(Module module, cl_status* err)
        : Kernel(module, "is_triangle_in_bin_test", err)
    {
    }

    cl_status load()
    {
        cl_status err = CL_SUCCESS;
        nr_uint argc = 0;
        if ((err = setArg(argc++, triangle_x)) != CL_SUCCESS) return err;
        if ((err = setArg(argc++, triangle_y)) != CL_SUCCESS) return err;
        if ((err = setArg(argc++, bin)) != CL_SUCCESS) return err;
        if ((err = setArg(argc++, dim)) != CL_SUCCESS) return err;
        return setArg(argc++, result);
    }

    Buffer triangle_x;
    Buffer triangle_y;
    Bin bin;
    ScreenDimension dim;
    Buffer result;
};

void testBin(TriangleInBin testee, CommandQueue q, const nr_uint dimension, const Bin& bin, const ScreenDimension& dim, nr_float* triangle_x, nr_float* triangle_y)
{
    cl_status err = CL_SUCCESS;

    cl_bool h_result = CL_FALSE;
    auto d_result = Buffer::make(defaultContext, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, 1, &h_result, &err);
    ASSERT_SUCCESS(err);

    nr_float defaultValue = -2.0;
    nr_int defaultValueAsInt = * (nr_int*) &defaultValue; 
    memset(triangle_x, defaultValueAsInt, 3 * sizeof(nr_float));
    memset(triangle_y, defaultValueAsInt, 3 * sizeof(nr_float));
    auto d_triangle_x = Buffer::make<nr_float>(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 3, triangle_x, &err);
    ASSERT_SUCCESS(err);
    auto d_triangle_y = Buffer::make<nr_float>(defaultContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 3, triangle_x, &err);
    ASSERT_SUCCESS(err);

    testee.bin = bin;
    testee.dim = dim;
    testee.triangle_x = d_triangle_x;
    testee.triangle_y = d_triangle_y;
    testee.result     = d_result;

    NDRange<1> global{1};
    NDRange<1> local{1};
    
    // Shouldn't be in any bin
    ASSERT_SUCCESS(testee.load());
    ASSERT_SUCCESS(q.enqueueKernelCommand<1>(testee, global, local));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_result, false, 1, &h_result));
    ASSERT_SUCCESS(q.await());

    // Middle of bin
    mkTriangleInCoords(bin.x + bin.width / 2, bin.y + bin.height / 2, dim, triangle_x, triangle_y);
    ASSERT_SUCCESS(q.enqueueBufferWriteCommand(d_triangle_x, false, 3, triangle_x));
    ASSERT_SUCCESS(q.enqueueBufferWriteCommand(d_triangle_y, false, 3, triangle_y));
    ASSERT_SUCCESS(q.await());
    ASSERT_SUCCESS(testee.load());
    ASSERT_SUCCESS(q.enqueueKernelCommand<1>(testee, global, local));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_result, false, 1, &h_result));
    ASSERT_SUCCESS(q.await());

    // Left-Low corner
    mkTriangleInCoords(bin.x, bin.y, dim, triangle_x, triangle_y);
    ASSERT_SUCCESS(q.enqueueBufferWriteCommand(d_triangle_x, false, 3, triangle_x));
    ASSERT_SUCCESS(q.enqueueBufferWriteCommand(d_triangle_y, false, 3, triangle_y));
    ASSERT_SUCCESS(q.await());
    ASSERT_SUCCESS(testee.load());
    ASSERT_SUCCESS(q.enqueueKernelCommand<1>(testee, global, local));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_result, false, 1, &h_result));
    ASSERT_SUCCESS(q.await());
}

TEST(Binning, IsSimplexInBin)
{
    const nr_uint dim = 5;

    cl_status err = CL_SUCCESS; 

    nr_float triangle_x[3];
    nr_float triangle_y[3];

    auto code = mkBinningModule(dim, 1, &err);
    ASSERT_SUCCESS(err);

    auto test = TriangleInBin(code, &err);
    ASSERT_SUCCESS(err) << utils::stringFromCLError(err);

    const ScreenDimension screenDim{1920, 1080};
    const Bin bin{32, 32, 800, 800};

    testBin(test, defaultCommandQueue, dim, bin, screenDim, triangle_x, triangle_y);
}

