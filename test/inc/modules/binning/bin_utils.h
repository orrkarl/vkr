#pragma once

#include <inc/includes.h>
#include <rendering/Render.h>

using namespace nr;
using namespace testing;
using namespace nr::__internal;

struct Bin
{
    NRuint width;
    NRuint height;
    NRuint x;
    NRuint y;
};

const Module::Option TEST_BINNING("_TEST_BINNING");

template<NRuint dim>
void mkTriangleInCoords(const NRuint x, const NRuint y, const ScreenDimension& screenDim, Triangle<dim>* result)
{
    result->points[0].values[0] = 2 * (x + 0.5) / (screenDim.width - 1) - 1;
    result->points[0].values[1] = 2 * (y + 0.5) / (screenDim.height - 1) - 1;

    for (NRuint i = 2; i < 3 * dim; ++i)
    {
        ((NRfloat*) result)[i] = -1.0f;
    }
}

void mkTriangleInCoords(const NRuint x, const NRuint y, const ScreenDimension& dim, float* triangle_x, float* triangle_y)
{
    triangle_x[0] = 2 * (x + 0.5) / (dim.width - 1) - 1;
    triangle_y[0] = 2 * (y + 0.5) / (dim.height - 1) - 1;
}

_nr::Module mkFineModule(const NRuint dim, cl_int* err)
{
    auto options = _nr::Module::Options
    {
        _nr::Module::CL_VERSION_12, 
        _nr::Module::WARNINGS_ARE_ERRORS, 
        _nr::Module::DEBUG, 
        TEST_BIN, 
        _nr::Module::RenderDimension(dim)
    };

    return _nr::Module({_nr::clcode::base, _nr::clcode::bin_rasterizer}, options, err);
}

Module::Option triangleTestCount(const NRuint triangleTestCount)
{
    return Module::Macro("TRIANGLE_TEST_COUNT", triangleTestCount);
}

Module mkBinningModule(const NRuint dim, cl_int* err)
{
    return mkStandardModule({_nr::clcode::base, _nr::clcode::bin_rasterizer}, dim, err);
}