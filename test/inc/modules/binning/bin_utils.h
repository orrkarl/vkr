#pragma once

#include <inc/includes.h>
#include <kernels/base.cl.h>
#include <kernels/bin_rasterizer.cl.h>
#include <rendering/Render.h>

using namespace nr;
using namespace testing;
using namespace nr::detail;

struct Bin
{
	nr_uint width;
	nr_uint height;
    nr_uint x;
    nr_uint y;
};

template <nr_uint QueueSize>
struct BinQueue
{
	nr_uint isEmpty;
	nr_uint queue[QueueSize];
};

template <nr_uint BinCountX, nr_uint BinCountY, nr_uint QueueSize>
using BinQueues = BinQueue<QueueSize>[BinCountX][BinCountY];

const Module::Macro TEST_BINNING("_TEST_BINNING");

template<nr_uint dim>
void mkTriangleInCoords(const nr_uint x, const nr_uint y, const ScreenDimension& screenDim, Triangle<dim>* result)
{
    result->points[0].values[0] = 2 * (x + 0.5f) / (screenDim.width - 1) - 1;
    result->points[0].values[1] = 2 * (y + 0.5f) / (screenDim.height - 1) - 1;

    for (nr_uint i = 2; i < 3 * dim; ++i)
    {
        ((nr_float*) result)[i] = -1.0f;
    }
}

void mkTriangleInCoords(const nr_uint x, const nr_uint y, const ScreenDimension& dim, float* triangle_x, float* triangle_y)
{
    triangle_x[0] = 2.0f * (x + 0.5f) / (dim.width - 1) - 1;
    triangle_y[0] = 2.0f * (y + 0.5f) / (dim.height - 1) - 1;
}

Module::Option mkTriangleTestCountMacro(const nr_uint triangleTestCount)
{
    return Module::Macro("TOTAL_TRIANGLE_COUNT", std::to_string(triangleTestCount));
}

Module mkBinningModule(const nr_uint dim, const nr_uint triangleTestCount, cl_status* err)
{
    auto opts = mkStandardOptions(dim);
    opts.push_back(TEST_BINNING);
    opts.push_back(mkTriangleTestCountMacro(triangleTestCount));
    auto ret = Module(defaultContext, Module::Sources{clcode::base, clcode::bin_rasterizer}, err);
    *err = ret.build(defaultDevice, opts);
    return ret;
}