#pragma once

#include <inc/includes.h>

#include "bin_utils.h"

#include <base/Module.h>
#include <kernels/base.cl.h>
#include <kernels/bin_rasterizer.cl.h>
#include <base/Buffer.h>
#include <rendering/Render.h>

using namespace nr;
using namespace nr::__internal;
using namespace testing;

class TriangleInBinParams
{
public:
    cl_int init(cl::CommandQueue queue) { return CL_SUCCESS; }
    
    cl_int load(cl::Kernel kernel)
    {
        cl_int err = CL_SUCCESS;
        NRuint argc = 0;
        if ((err = kernel.setArg(argc++, triangle_x.getBuffer())) != CL_SUCCESS) return err;
        if ((err = kernel.setArg(argc++, triangle_y.getBuffer())) != CL_SUCCESS) return err;
        if ((err = kernel.setArg(argc++, bin)) != CL_SUCCESS) return err;
        if ((err = kernel.setArg(argc++, dim)) != CL_SUCCESS) return err;
        return kernel.setArg(argc++, result.getBuffer());
    }

    Buffer triangle_x;
    Buffer triangle_y;
    Bin bin;
    ScreenDimension dim;
    Buffer result;
};

void testBin(Kernel<TriangleInBinParams> testee, cl::CommandQueue q, const NRuint dimension, const Bin& bin, const ScreenDimension& dim, NRfloat* triangle_x, NRfloat* triangle_y)
{
    cl_int err = CL_SUCCESS;

    cl_bool h_result = CL_FALSE;
    Buffer d_result(CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_bool), &h_result, &err);
    ASSERT_PRED1(error::isSuccess, err);

    NRfloat defaultValue = -2.0;
    NRint defaultValueAsInt = * (NRint*) &defaultValue; 
    memset(triangle_x, defaultValueAsInt, 3 * sizeof(NRfloat));
    memset(triangle_y, defaultValueAsInt, 3 * sizeof(NRfloat));
    Buffer d_triangle_x(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 3 * sizeof(NRfloat), triangle_x, &err);
    ASSERT_PRED1(error::isSuccess, err);
    Buffer d_triangle_y(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 3 * sizeof(NRfloat), triangle_x, &err);
    ASSERT_PRED1(error::isSuccess, err);

    testee.params.bin = bin;
    testee.params.dim = dim;
    testee.params.triangle_x = d_triangle_x;
    testee.params.triangle_y = d_triangle_y;
    testee.params.result     = d_result;
    testee.global = cl::NDRange(1);
    testee.local  = cl::NDRange(1);

    // Shouldn't be in any bin
    ASSERT_EQ(CL_SUCCESS, testee(q));
    ASSERT_EQ(CL_SUCCESS, q.enqueueReadBuffer(d_result.getBuffer(), CL_TRUE, 0, sizeof(cl_bool), &h_result));
    ASSERT_EQ(CL_SUCCESS, q.finish());
    ASSERT_EQ(CL_FALSE, h_result) << "Sanity check";

    // Middle of bin
    mkTriangleInCoords(bin.x + bin.width / 2, bin.y + bin.height / 2, dim, triangle_x, triangle_y);
    ASSERT_EQ(CL_SUCCESS, q.enqueueWriteBuffer(d_triangle_x.getBuffer(), CL_TRUE, 0, 3 * sizeof(NRfloat), triangle_x));
    ASSERT_EQ(CL_SUCCESS, q.enqueueWriteBuffer(d_triangle_y.getBuffer(), CL_TRUE, 0, 3 * sizeof(NRfloat), triangle_y));
    ASSERT_EQ(CL_SUCCESS, testee(q));
    ASSERT_EQ(CL_SUCCESS, q.enqueueReadBuffer(d_result.getBuffer(), CL_TRUE, 0, sizeof(cl_bool), &h_result));
    ASSERT_EQ(CL_SUCCESS, q.finish());
    ASSERT_EQ(CL_TRUE, h_result) << "Middle of the bin";

    // Left-Low corner
    mkTriangleInCoords(bin.x, bin.y, dim, triangle_x, triangle_y);
    ASSERT_EQ(CL_SUCCESS, q.enqueueWriteBuffer(d_triangle_x.getBuffer(), CL_TRUE, 0, 3 * sizeof(NRfloat), triangle_x));
    ASSERT_EQ(CL_SUCCESS, q.enqueueWriteBuffer(d_triangle_y.getBuffer(), CL_TRUE, 0, 3 * sizeof(NRfloat), triangle_y));
    ASSERT_EQ(CL_SUCCESS, testee(q));
    ASSERT_EQ(CL_SUCCESS, q.enqueueReadBuffer(d_result.getBuffer(), CL_TRUE, 0, sizeof(cl_bool), &h_result));
    ASSERT_EQ(CL_SUCCESS, q.finish());
    ASSERT_EQ(CL_TRUE, h_result) << "Left-Low corner of the bin";
    
    // Just above top-left corner of the bin
    mkTriangleInCoords(bin.x, bin.y + bin.height + 1, dim, triangle_x, triangle_y);
    ASSERT_EQ(CL_SUCCESS, q.enqueueWriteBuffer(d_triangle_x.getBuffer(), CL_TRUE, 0, 3 * sizeof(NRfloat), triangle_x));
    ASSERT_EQ(CL_SUCCESS, q.enqueueWriteBuffer(d_triangle_y.getBuffer(), CL_TRUE, 0, 3 * sizeof(NRfloat), triangle_y));
    ASSERT_EQ(CL_SUCCESS, testee(q));
    ASSERT_EQ(CL_SUCCESS, q.enqueueReadBuffer(d_result.getBuffer(), CL_TRUE, 0, sizeof(cl_bool), &h_result));
    ASSERT_EQ(CL_SUCCESS, q.finish());
    ASSERT_EQ(CL_FALSE, h_result) << "Above top left corner of the bin";
}

TEST(Binning, IsSimplexInBin)
{
    const NRuint dim = 5;

    const char options_fmt[] = "-cl-std=CL2.0 -Werror -D _TEST_BINNING -D RENDER_DIMENSION=%d -D TRIANGLE_TEST_COUNT=1";
    char options[sizeof(options_fmt) * 2];
    memset(options, 0, sizeof(options));

    sprintf(options, options_fmt, dim);

    cl_int err = CL_SUCCESS; 

    NRfloat triangle_x[3];
    NRfloat triangle_y[3];

    Module code({clcode::base, clcode::bin_rasterizer}, options, &err);
    ASSERT_EQ(CL_SUCCESS, err);

    auto test = code.makeKernel<TriangleInBinParams>("is_triangle_in_bin_test", &err);
    ASSERT_EQ(CL_SUCCESS, err) << utils::stringFromCLError(err);

    const ScreenDimension screenDim{1920, 1080};
    const Bin bin{32, 32, 800, 800};

    testBin(test, cl::CommandQueue::getDefault(), dim, bin, screenDim, triangle_x, triangle_y);
}

