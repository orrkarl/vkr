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


class ReduceTriangleBuffer : public Kernel
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

    Buffer<nr_float> triangles;
    nr_uint offset;
    Buffer<nr_float> result;
};

template<nr_uint dim>
void generateTriangleData(const nr_uint triangleCount, Triangle<dim>* buffer)
{
    nr_float diff = 2.0f / (triangleCount - 1);
    nr_float base = -1;

    for (nr_uint i = 0; i < triangleCount; ++i)
    {
        for (nr_uint j = 0; j < 3; ++j)
        {
            buffer[i].points[j].values[0] = base + i * diff;
            buffer[i].points[j].values[1] = base + i * diff;

            for (nr_uint k = 2; k < dim - 2; ++k)
            {
                buffer[i].points[j].values[k] = -2;
            }
        }
    }
}

template<nr_uint dim>
void extractNDCPosition(const nr_uint triangleCount, const Triangle<dim>* buffer, nr_float* result)
{
    for (nr_uint i = 0; i < triangleCount * 3; ++i)
    {
        result[2 * i] = ((const Point<dim>*) buffer)[i].values[0];
        result[2 * i + 1] = ((const Point<dim>*) buffer)[i].values[1];
    }
}


TEST(Binning, ReduceTriangleBuffer)
{
    const nr_uint dim = 5;
    const nr_uint triangleCount = 3;
    const nr_uint offset = 2;
    
    cl_status err = CL_SUCCESS; 

    auto code = mkBinningModule(dim, triangleCount, &err);
    ASSERT_SUCCESS(err);

    auto q = CommandQueue::getDefault();

    Triangle<dim> h_triangles_raw[triangleCount + offset];
    Triangle<dim>* h_triangles = h_triangles_raw + offset;
    const nr_uint triangleFloatCount = (sizeof(h_triangles_raw) - offset * sizeof(Triangle<dim>)) / sizeof(nr_float);

    generateTriangleData<dim>(triangleCount, h_triangles);

    const nr_uint expectedFloatCount = triangleCount * 3 * 2;
    nr_float h_expected[expectedFloatCount];
    extractNDCPosition<dim>(triangleCount, h_triangles, h_expected);

    std::vector<nr_float> h_actual(expectedFloatCount);

    Buffer<nr_float> d_triangle(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(h_triangles_raw) / sizeof(nr_float), (nr_float*) h_triangles_raw, &err);
    ASSERT_SUCCESS(err);
    Buffer<nr_float> d_result(CL_MEM_WRITE_ONLY, 2 * triangleFloatCount / dim, &err);
    ASSERT_SUCCESS(err);
    
    auto test = ReduceTriangleBuffer(code, &err);
    ASSERT_SUCCESS(err);

    test.triangles = d_triangle;
    test.offset    = offset * sizeof(Triangle<dim>) / sizeof(nr_float);
    test.result    = d_result;
    
    std::array<size_t, 1> local  = { 30 };
    std::array<size_t, 1> global = { 1 };

    ASSERT_SUCCESS(test.load());
    ASSERT_SUCCESS(q.enqueueKernelCommand<1>(test, global, local));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_result, false, expectedFloatCount, h_actual.data()));
    ASSERT_SUCCESS(q.await());

    ASSERT_THAT(h_actual, ElementsAreArray(h_expected, sizeof(h_expected) / sizeof(nr_float)));
}

