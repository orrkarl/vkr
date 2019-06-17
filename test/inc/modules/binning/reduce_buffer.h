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

template<NRuint dim>
void generateTriangleData(const NRuint triangleCount, Triangle<dim>* buffer)
{
    NRfloat diff = 2.0 / (triangleCount - 1);
    NRfloat base = -1;

    for (NRuint i = 0; i < triangleCount; ++i)
    {
        for (NRuint j = 0; j < 3; ++j)
        {
            buffer[i].points[j].values[0] = base + i * diff;
            buffer[i].points[j].values[1] = base + i * diff;

            for (NRuint k = 2; k < dim - 2; ++k)
            {
                buffer[i].points[j].values[k] = -2;
            }
        }
    }
}

template<NRuint dim>
void extractNDCPosition(const NRuint triangleCount, const Triangle<dim>* buffer, NRfloat* result)
{
    for (NRuint i = 0; i < triangleCount * 3; ++i)
    {
        result[2 * i] = ((const Point<dim>*) buffer)[i].values[0];
        result[2 * i + 1] = ((const Point<dim>*) buffer)[i].values[1];
    }
}


TEST(Binning, ReduceTriangleBuffer)
{
    const NRuint dim = 5;
    const NRuint triangleCount = 3;
    const NRuint offset = 2;
    
    Module::Options options = 
    {
        Module::CL_VERSION_12, 
        Module::WARNINGS_ARE_ERRORS, 
        Module::RenderDimension(dim), 
        Moduke::DEBUG, 
        Module::Macro("_TEST_BINNING"), 
        Module::Macro("TRIANGLE_TEST_COUNT", 
        std::to_string(dim))
    };

    cl_int err = CL_SUCCESS; 

    Module code({clcode::base, clcode::bin_rasterizer}, options, &err);
    ASSERT_EQ(CL_SUCCESS, err);

    Triangle<dim> h_triangles_raw[triangleCount + offset];
    Triangle<dim>* h_triangles = h_triangles_raw + offset;
    const NRuint trianglesSize = sizeof(h_triangles_raw) - offset * sizeof(Triangle<dim>);

    generateTriangleData<dim>(triangleCount, h_triangles);

    NRfloat h_expected[triangleCount * 3 * 2];
    extractNDCPosition<dim>(triangleCount, h_triangles, h_expected);

    std::vector<NRfloat> h_actual(triangleCount * 3 * 2);

    Buffer d_triangle(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(h_triangles_raw), (float*) h_triangles_raw, &err);
    ASSERT_PRED1(error::isSuccess, err);
    Buffer d_result(CL_MEM_WRITE_ONLY, 2 * trianglesSize / dim, &err);
    ASSERT_PRED1(error::isSuccess, err);
    
    Kernel<ReduceTriangleBufferParams> test = code.makeKernel<ReduceTriangleBufferParams>("reduce_triangle_buffer_test", &err);
    ASSERT_EQ(CL_SUCCESS, err) << utils::stringFromCLError(err);

    test.params.triangles = d_triangle;
    test.params.offset    = offset * sizeof(Triangle<dim>) / sizeof(NRfloat);
    test.params.result    = d_result;
    test.local  = cl::NDRange(30);
    test.global = cl::NDRange(1);

    ASSERT_EQ(CL_SUCCESS, test(cl::CommandQueue::getDefault()));

    ASSERT_EQ(CL_SUCCESS, cl::CommandQueue::getDefault().enqueueReadBuffer(d_result.getBuffer(), CL_FALSE, 0, sizeof(h_expected), h_actual.data()));
    
    ASSERT_EQ(CL_SUCCESS, cl::CommandQueue::getDefault().finish());

    ASSERT_THAT(h_actual, ElementsAreArray(h_expected, sizeof(h_expected) / sizeof(NRfloat)));
}

