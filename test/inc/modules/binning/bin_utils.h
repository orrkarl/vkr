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

const Module::Macro TEST_BINNING("_TEST_BINNING");

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

Module::Option mkTriangleTestCountMacro(const NRuint triangleTestCount)
{
    return Module::Macro("TRIANGLE_TEST_COUNT", std::to_string(triangleTestCount));
}

Module mkBinningModule(const NRuint dim, const NRuint triangleTestCount, cl_int* err)
{
    auto opts = mkStandardOptions(dim);
    opts.push_back(TEST_BINNING);
    opts.push_back(mkTriangleTestCountMacro(triangleTestCount));
    return Module({clcode::base, clcode::bin_rasterizer}, opts, err);
}