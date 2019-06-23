#pragma once

#include <inc/includes.h>

#include <base/Module.h>
#include <kernels/base.cl.h>
#include <kernels/bin_rasterizer.cl.h>
#include <base/Buffer.h>

#include "bin_utils.h"

using namespace nr;
using namespace nr::__internal;
using namespace testing;


class ReduceTriangleBuffer : Kernel
{
public:
    ReduceTriangleBuffer(Module module, cl_status* err)
        : Kernel(module, "reduce_triangle_buffer_test", err)
    {
    }

    cl_status load()
    {
        cl_status err = CL_SUCCESS;
        if ((err = setArg(0, triangles)) != CL_SUCCESS) return err;
        if ((err = setArg(1, offset)) != CL_SUCCESS) return err;
        return setArg(2, result);
    }

    Buffer<NRfloat> triangles;
    NRuint offset;
    Buffer<NRfloat> result;
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
    
    cl_status err = CL_SUCCESS; 

    auto code = mkBinningModule(dim, triangleCount, &err);
    ASSERT_SUCCESS(err);

    Triangle<dim> h_triangles_raw[triangleCount + offset];
    Triangle<dim>* h_triangles = h_triangles_raw + offset;
    const NRuint triangleFloatCount = (sizeof(h_triangles_raw) - offset * sizeof(Triangle<dim>)) / sizeof(NRfloat);

    generateTriangleData<dim>(triangleCount, h_triangles);

    const NRuint expectedFloatCount = triangleCount * 3 * 2;
    NRfloat h_expected[expectedFloatCount];
    extractNDCPosition<dim>(triangleCount, h_triangles, h_expected);

    std::vector<NRfloat> h_actual(expectedFloatCount);

    Buffer<NRfloat> d_triangle(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(h_triangles_raw) / sizeof(NRfloat), (NRfloat*) h_triangles_raw, &err);
    ASSERT_SUCCESS(err);
    Buffer<NRfloat> d_result(CL_MEM_WRITE_ONLY, 2 * triangleFloatCount / dim, &err);
    ASSERT_SUCCESS(err);
    
    auto test = ReduceTriangleBuffer(code, &err);
    ASSERT_SUCCESS(err);

    test.triangles = d_triangle;
    test.offset    = offset * sizeof(Triangle<dim>) / sizeof(NRfloat);
    test.result    = d_result;
    
    std::array<NRuint, 1> local  = { 30 };
    std::array<NRuint, 1> global = { 1 };

    ASSERT_SUCCESS(test.load());
    ASSERT_SUCCESS(CommandQueue::getDefault().enqueueBufferReadCommand(d_result, false, expectedFloatCount, h_actual.data()));
    ASSERT_SUCCESS(CommandQueue::getDefault().await());

    ASSERT_THAT(h_actual, ElementsAreArray(h_expected, sizeof(h_expected) / sizeof(NRfloat)));
}

