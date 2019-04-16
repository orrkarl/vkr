#pragma once

#include <inc/includes.h>

#include <base/Module.h>
#include <kernels/base.cl.h>
#include <kernels/bin_rasterizer.cl.h>
#include <pipeline/BinRasterizer.h>
#include <base/Buffer.h>

using namespace nr;
using namespace nr::__internal;
using namespace testing;

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

TEST(Binning, FullRasterizer)
{
    cl_int err = CL_SUCCESS; 
    Error error = Error::NO_ERROR;

    const NRuint dim = 4;
    const NRuint simplexCount = 5;

    Simplex<dim> h_simplices[simplexCount];
    const NRuint workGroupCount = 8;
    const ScreenDimension dim{1920, 1080};
    const NRuint binWidth = 32;
    const NRuint binHeight = 32;


    const char options_fmt[] = "-cl-std=CL2.0 -Werror -D _TEST_BINNING -D RENDER_DIMENSION=%d -D SIMPLEX_TEST_COUNT=%d";
    char options[sizeof(options_fmt) * 2];
    memset(options, 0, sizeof(options));

    sprintf(options, options_fmt, dim, simplexCount);

}