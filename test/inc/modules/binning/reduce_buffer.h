#pragma once

#include <inc/includes.h>

#include <base/Module.h>
#include <kernels/base.cl.h>
#include <kernels/bin_rasterizer.cl.h>
#include <base/Buffer.h>

using namespace nr;
using namespace nr::__internal;
using namespace testing;


class ReduceSimplexBufferParams
{
public:
    cl_int init(cl::CommandQueue queue) { return CL_SUCCESS; }
    
    cl_int load(cl::Kernel kernel)
    {
        cl_int err = CL_SUCCESS;
        if ((err = kernel.setArg(0, simplices.getBuffer())) != CL_SUCCESS) return err;
        if ((err = kernel.setArg(1, offset)) != CL_SUCCESS) return err;
        return kernel.setArg(2, result.getBuffer());
    }

    Buffer simplices = Buffer(Type::FLOAT);
    NRuint offset;
    Buffer result = Buffer(Type::BOOL);
};

template<NRuint Dim>
struct Point
{
    NRfloat values[Dim];
};

template<NRuint Dim>
struct Simplex
{
    Point<Dim> points[Dim];
};

template<NRuint dim>
void generateSimplexData(const NRuint simplexCount, Simplex<dim>* buffer)
{
    NRfloat diff = 2.0 / (simplexCount - 1);
    NRfloat base = -1;

    for (NRuint i = 0; i < simplexCount; ++i)
    {
        for (NRuint j = 0; j < dim; ++j)
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
void extractNDCPosition(const NRuint simplexCount, const Simplex<dim>* buffer, NRfloat* result)
{
    for (NRuint i = 0; i < simplexCount * dim; ++i)
    {
        result[2 * i] = ((const Point<dim>*) buffer)[i].values[0];
        result[2 * i + 1] = ((const Point<dim>*) buffer)[i].values[1];
    }
}


TEST(Binning, ReduceSimplexBuffer)
{
    const NRuint dim = 5;
    const NRuint simplexCount = 3;
    const NRuint offset = 2;

    const char options_fmt[] = "-cl-std=CL2.0 -Werror -D _DEBUG -D _TEST_BINNING -D RENDER_DIMENSION=%d -D SIMPLEX_TEST_COUNT=%d";
    char options[sizeof(options_fmt) * 2];
    memset(options, 0, sizeof(options));

    sprintf(options, options_fmt, dim, simplexCount);

    cl_int err = CL_SUCCESS; 
    Error error = Error::NO_ERROR;

    Module code({clcode::base, clcode::bin_rasterizer}, options, &err);
    ASSERT_EQ(CL_SUCCESS, err);

    Simplex<dim> h_simplices_raw[simplexCount + offset];
    Simplex<dim>* h_simplices = h_simplices_raw + offset;
    const NRuint simplicesSize = sizeof(h_simplices_raw) - offset * sizeof(Simplex<dim>);

    generateSimplexData<dim>(simplexCount, h_simplices);

    NRfloat h_expected[simplexCount * dim * 2];
    extractNDCPosition<dim>(simplexCount, h_simplices, h_expected);

    std::vector<NRfloat> h_actual(simplexCount * dim * 2);

    Buffer d_simplex(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(h_simplices_raw), (float*) h_simplices_raw, &error);
    ASSERT_PRED1(error::isSuccess, error);
    Buffer d_result(CL_MEM_WRITE_ONLY, 2 * simplicesSize / dim, Type::FLOAT, &error);
    ASSERT_PRED1(error::isSuccess, error);
    
    Kernel<ReduceSimplexBufferParams> test = code.makeKernel<ReduceSimplexBufferParams>("reduce_simplex_buffer_test", &err);
    ASSERT_EQ(CL_SUCCESS, err) << utils::stringFromCLError(err);

    test.params.simplices = d_simplex;
    test.params.offset    = offset * sizeof(Simplex<dim>) / sizeof(NRfloat);
    test.params.result    = d_result;
    test.local  = cl::NDRange(30);
    test.global = cl::NDRange(1);

    ASSERT_EQ(CL_SUCCESS, test(cl::CommandQueue::getDefault()));

    ASSERT_EQ(CL_SUCCESS, cl::CommandQueue::getDefault().enqueueReadBuffer(d_result.getBuffer(), CL_FALSE, 0, sizeof(h_expected), h_actual.data()));
    
    ASSERT_EQ(CL_SUCCESS, cl::CommandQueue::getDefault().finish());

    ASSERT_THAT(h_actual, ElementsAreArray(h_expected, sizeof(h_expected) / sizeof(NRfloat)));
}

