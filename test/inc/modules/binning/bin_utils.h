#pragma once

#include <inc/includes.h>
#include <rendering/Render.h>

using namespace nr;
using namespace testing;


struct Bin
{
    NRuint width;
    NRuint height;
    NRuint x;
    NRuint y;
};


template<NRuint dim>
void mkSimplexInCoords(const NRuint x, const NRuint y, const ScreenDimension& screenDim, Simplex<dim>* result)
{
    result->points[0].values[0] = 2 * (x + 0.5) / (screenDim.width - 1) - 1;
    result->points[0].values[1] = 2 * (y + 0.5) / (screenDim.height - 1) - 1;

    for (NRuint i = 2; i < dim * dim; ++i)
    {
        ((NRfloat*) result)[i] = -1.0f;
    }
}

void mkSimplexInCoords(const NRuint x, const NRuint y, const ScreenDimension& dim, float* simplex_x, float* simplex_y)
{
    simplex_x[0] = 2 * (x + 0.5) / (dim.width - 1) - 1;
    simplex_y[0] = 2 * (y + 0.5) / (dim.height - 1) - 1;
}
