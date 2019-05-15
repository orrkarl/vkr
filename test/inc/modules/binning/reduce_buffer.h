#pragma once

#include <inc/includes.h>

#include <base/Module.h>
#include <kernels/base.cl.h>
#include <kernels/bin_rasterizer.cl.h>
#include <base/Buffer.h>

using namespace nr;
using namespace nr::__internal;
using namespace testing;


class ReduceTriangleBufferParams
{
public:
    cl_int init(cl::CommandQueue queue) { return CL_SUCCESS; }
    
    cl_int load(cl::Kernel kernel)
    {
        cl_int err = CL_SUCCESS;
        if ((err = kernel.setArg(0, triangles.getBuffer())) != CL_SUCCESS) return err;
        if ((err = kernel.setArg(1, offset)) != CL_SUCCESS) return err;
        return kernel.setArg(2, result.getBuffer());
    }

    Buffer triangles;
    NRuint offset;
    Buffer result;
};

void generateTriangleData(const NRuint triangleCount, Triangle* buffer)
{
    NRfloat diff = 2.0 / (triangleCount - 1);
    NRfloat base = -1;

    for (NRuint i = 0; i < triangleCount; ++i)
    {
        for (NRuint j = 0; j < 3; ++j)
        {
            buffer[i].points[j].values[0] = base + i * diff;
            buffer[i].points[j].values[1] = base + i * diff;
            buffer[i].points[j].values[2] = -2;
        }
    }
}

void extractNDCPosition(const NRuint triangleCount, const Triangle* buffer, NRfloat* result)
{
    for (NRuint i = 0; i < triangleCount * 3; ++i)
    {
        result[2 * i] = ((const Point<3>*) buffer)[i].values[0];
        result[2 * i + 1] = ((const Point<3>*) buffer)[i].values[1];
    }
}


TEST(Binning, ReduceTriangleBuffer)
{
    const NRuint dim = 5;
    const NRuint triangleCount = 3;
    const NRuint offset = 2;

    const char options_fmt[] = "-cl-std=CL2.0 -Werror -D _DEBUG -D _TEST_BINNING -D RENDER_DIMENSION=%d -D TRIANGLE_TEST_COUNT=%d";
    char options[sizeof(options_fmt) * 2];
    memset(options, 0, sizeof(options));

    sprintf(options, options_fmt, dim, triangleCount);

    cl_int err = CL_SUCCESS; 
    Error error = Error::NO_ERROR;

    Module code({clcode::base, clcode::bin_rasterizer}, options, &err);
    ASSERT_EQ(CL_SUCCESS, err);

    Triangle h_triangles_raw[triangleCount + offset];
    Triangle* h_triangles = h_triangles_raw + offset;
    const NRuint trianglesSize = sizeof(h_triangles_raw) - offset * sizeof(Triangle);

    generateTriangleData(triangleCount, h_triangles);

    NRfloat h_expected[triangleCount * 3 * 2];
    extractNDCPosition(triangleCount, h_triangles, h_expected);

    std::vector<NRfloat> h_actual(triangleCount * 3 * 2);

    Buffer d_triangle(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(h_triangles_raw), (float*) h_triangles_raw, &error);
    ASSERT_PRED1(error::isSuccess, error);
    Buffer d_result(CL_MEM_WRITE_ONLY, 2 * trianglesSize / 3, &error);
    ASSERT_PRED1(error::isSuccess, error);
    
    Kernel<ReduceTriangleBufferParams> test = code.makeKernel<ReduceTriangleBufferParams>("reduce_triangle_buffer_test", &err);
    ASSERT_EQ(CL_SUCCESS, err) << utils::stringFromCLError(err);

    test.params.triangles = d_triangle;
    test.params.offset    = offset * sizeof(Triangle) / sizeof(NRfloat);
    test.params.result    = d_result;
    test.local  = cl::NDRange(30);
    test.global = cl::NDRange(1);

    ASSERT_EQ(CL_SUCCESS, test(cl::CommandQueue::getDefault()));

    ASSERT_EQ(CL_SUCCESS, cl::CommandQueue::getDefault().enqueueReadBuffer(d_result.getBuffer(), CL_FALSE, 0, sizeof(h_expected), h_actual.data()));
    
    ASSERT_EQ(CL_SUCCESS, cl::CommandQueue::getDefault().finish());

    ASSERT_THAT(h_actual, ElementsAreArray(h_expected, sizeof(h_expected) / sizeof(NRfloat)));
}

