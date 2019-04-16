#pragma once

#include <inc/includes.h>

#include <base/Module.h>
#include <kernels/base.cl.h>
#include <kernels/bin_rasterizer.cl.h>
#include <base/Buffer.h>
#include <rendering/RenderState.h>

using namespace nr;
using namespace nr::__internal;
using namespace testing;

struct Bin
{
    NRuint width;
    NRuint height;
    NRuint x;
    NRuint y;
};

class SimplexInBinParams
{
public:
    cl_int init(cl::CommandQueue queue) { return CL_SUCCESS; }
    
    cl_int load(cl::Kernel kernel)
    {
        cl_int err = CL_SUCCESS;
        NRuint argc = 0;
        if ((err = kernel.setArg(argc++, simplex_x.getBuffer())) != CL_SUCCESS) return err;
        if ((err = kernel.setArg(argc++, simplex_y.getBuffer())) != CL_SUCCESS) return err;
        if ((err = kernel.setArg(argc++, bin)) != CL_SUCCESS) return err;
        if ((err = kernel.setArg(argc++, dim)) != CL_SUCCESS) return err;
        return kernel.setArg(argc++, result.getBuffer());
    }

    Buffer simplex_x = Buffer(Type::FLOAT);
    Buffer simplex_y = Buffer(Type::FLOAT);
    Bin bin;
    ScreenDimension dim;
    Buffer result = Buffer(Type::BOOL);
};

void mkSimplexInCoords(const NRuint x, const NRuint y, const ScreenDimension& dim, float* simplex_x, float* simplex_y)
{
    simplex_x[0] = 2 * (x + 0.5) / (dim.width - 1) - 1;
    simplex_y[0] = 2 * (y + 0.5) / (dim.height - 1) - 1;
}

void testBin(Kernel<SimplexInBinParams> testee, cl::CommandQueue q, const NRuint dimension, const Bin& bin, const ScreenDimension& dim, NRfloat* simplex_x, NRfloat* simplex_y)
{
    Error error = Error::NO_ERROR;
    cl_int err = CL_SUCCESS;

    cl_bool h_result = CL_FALSE;
    Buffer d_result(CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_bool), &h_result, &error);
    ASSERT_PRED1(error::isSuccess, error);

    NRfloat defaultValue = -2.0;
    NRint defaultValueAsInt = * (NRint*) &defaultValue; 
    memset(simplex_x, defaultValueAsInt, dimension * 2 * sizeof(NRfloat));
    memset(simplex_y, defaultValueAsInt, dimension * 2 * sizeof(NRfloat));
    Buffer d_simplex_x(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, dimension * 2 * sizeof(NRfloat), simplex_x, &error);
    ASSERT_PRED1(error::isSuccess, error);
    Buffer d_simplex_y(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, dimension * 2 * sizeof(NRfloat), simplex_y, &error);
    ASSERT_PRED1(error::isSuccess, error);

    testee.params.bin = bin;
    testee.params.dim = dim;
    testee.params.simplex_x = d_simplex_x;
    testee.params.simplex_y = d_simplex_y;
    testee.params.result    = d_result;
    testee.global = cl::NDRange(1);
    testee.local  = cl::NDRange(1);

    // Shouldn't be in any bin
    ASSERT_EQ(CL_SUCCESS, testee(q));
    ASSERT_EQ(CL_SUCCESS, q.enqueueReadBuffer(d_result.getBuffer(), CL_TRUE, 0, sizeof(cl_bool), &h_result));
    ASSERT_EQ(CL_SUCCESS, q.finish());
    ASSERT_EQ(CL_FALSE, h_result) << "Sanity check";

    // Middle of bin
    mkSimplexInCoords(bin.x + bin.width / 2, bin.y + bin.height / 2, dim, simplex_x, simplex_y);
    ASSERT_EQ(CL_SUCCESS, q.enqueueWriteBuffer(d_simplex_x.getBuffer(), CL_TRUE, 0, dimension * 2 * sizeof(NRfloat), simplex_x));
    ASSERT_EQ(CL_SUCCESS, q.enqueueWriteBuffer(d_simplex_y.getBuffer(), CL_TRUE, 0, dimension * 2 * sizeof(NRfloat), simplex_y));
    ASSERT_EQ(CL_SUCCESS, testee(q));
    ASSERT_EQ(CL_SUCCESS, q.enqueueReadBuffer(d_result.getBuffer(), CL_TRUE, 0, sizeof(cl_bool), &h_result));
    ASSERT_EQ(CL_SUCCESS, q.finish());
    ASSERT_EQ(CL_TRUE, h_result) << "Middle of the bin";

    // Left-Low corner
    mkSimplexInCoords(bin.x, bin.y, dim, simplex_x, simplex_y);
    ASSERT_EQ(CL_SUCCESS, q.enqueueWriteBuffer(d_simplex_x.getBuffer(), CL_TRUE, 0, dimension * 2 * sizeof(NRfloat), simplex_x));
    ASSERT_EQ(CL_SUCCESS, q.enqueueWriteBuffer(d_simplex_y.getBuffer(), CL_TRUE, 0, dimension * 2 * sizeof(NRfloat), simplex_y));
    ASSERT_EQ(CL_SUCCESS, testee(q));
    ASSERT_EQ(CL_SUCCESS, q.enqueueReadBuffer(d_result.getBuffer(), CL_TRUE, 0, sizeof(cl_bool), &h_result));
    ASSERT_EQ(CL_SUCCESS, q.finish());
    ASSERT_EQ(CL_TRUE, h_result) << "Left-Low corner of the bin";
    
    // Just above top-left corner of the bin
    mkSimplexInCoords(bin.x, bin.y + bin.height + 1, dim, simplex_x, simplex_y);
    ASSERT_EQ(CL_SUCCESS, q.enqueueWriteBuffer(d_simplex_x.getBuffer(), CL_TRUE, 0, dimension * 2 * sizeof(NRfloat), simplex_x));
    ASSERT_EQ(CL_SUCCESS, q.enqueueWriteBuffer(d_simplex_y.getBuffer(), CL_TRUE, 0, dimension * 2 * sizeof(NRfloat), simplex_y));
    ASSERT_EQ(CL_SUCCESS, testee(q));
    ASSERT_EQ(CL_SUCCESS, q.enqueueReadBuffer(d_result.getBuffer(), CL_TRUE, 0, sizeof(cl_bool), &h_result));
    ASSERT_EQ(CL_SUCCESS, q.finish());
    ASSERT_EQ(CL_FALSE, h_result) << "Above top left corner of the bin";
}

TEST(Binning, IsSimplexInBin)
{
    const NRuint dim = 5;
    const NRuint simplexCount = 1;

    const char options_fmt[] = "-cl-std=CL2.0 -Werror -D _TEST_BINNING -D RENDER_DIMENSION=%d -D SIMPLEX_TEST_COUNT=%d";
    char options[sizeof(options_fmt) * 2];
    memset(options, 0, sizeof(options));

    sprintf(options, options_fmt, dim, simplexCount);

    cl_int err = CL_SUCCESS; 
    Error error = Error::NO_ERROR;

    NRfloat simplex_x[dim * 2];
    NRfloat simplex_y[dim * 2];

    Module code({clcode::base, clcode::bin_rasterizer}, options, &err);
    ASSERT_EQ(CL_SUCCESS, err);

    auto test = code.makeKernel<SimplexInBinParams>("is_simplex_in_bin_test", &err);
    ASSERT_EQ(CL_SUCCESS, err) << utils::stringFromCLError(err);

    const ScreenDimension screenDim{1920, 1080};
    const Bin bin{32, 32, 800, 800};

    testBin(test, cl::CommandQueue::getDefault(), dim, bin, screenDim, simplex_x, simplex_y);
}

