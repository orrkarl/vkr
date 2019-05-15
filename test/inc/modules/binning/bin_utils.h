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
