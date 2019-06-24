#pragma once

#include <inc/includes.h>
#include <inc/modules/binning/bin_utils.h>
#include <rendering/Render.h>
#include <pipeline/BinRasterizer.h>

const RawColorRGB RED = { 255, 0, 0 };
const nr::Module::Macro TEST_FINE("_TEST_FINE");

template<nr_uint dim>
void mkTriangleInCoordinates(const NDCPosition p0, const NDCPosition p1, const NDCPosition p2, Triangle<dim>* triangle)
{
    mkTriangleInCoordinates(p0, p1, p2, 0, triangle);
}

template<nr_uint dim>
void mkTriangleInCoordinates(const NDCPosition p0, const NDCPosition p1, const NDCPosition p2, const nr_float minDistance, Triangle<dim>* triangle)
{
    mkTriangleInExactCoordinates(p0, p1, p2, minDistance + (1 - minDistance) * rand() / RAND_MAX, triangle);
}

template<nr_uint dim>
void mkTriangleInExactCoordinates(const NDCPosition p0, const NDCPosition p1, const NDCPosition p2, const nr_float distance, Triangle<dim>* triangle)
{
    triangle->points[0].values[0] = p0.x;
    triangle->points[0].values[1] = p0.y;

    triangle->points[1].values[0] = p1.x;
    triangle->points[1].values[1] = p1.y;
    
    triangle->points[2].values[0] = p2.x;
    triangle->points[2].values[1] = p2.y;

    for (nr_uint i = 0; i < 3; ++i)
    {
        for (nr_uint j = 2; j < dim; ++j)
        {
            triangle->points[i].values[j] = distance;
        }
    }
}

template<nr_uint dim>
void mkTriangleFullyInBin(const ScreenDimension& screenDim, const Bin& bin, const nr_float distance, const nr_uint index, Triangle<dim>* triangle)
{
    ScreenPosition top_left_screen     = { bin.x, bin.y + bin.height - 1 };
    ScreenPosition bottom_left_screen  = { bin.x, bin.y };
    ScreenPosition bottom_right_screen = { bin.x + bin.width - 1, bin.y };

    NDCPosition top_left     = ndcFromScreen(top_left_screen, screenDim);
    NDCPosition bottom_left  = ndcFromScreen(bottom_left_screen, screenDim);
    NDCPosition bottom_right = ndcFromScreen(bottom_right_screen, screenDim);

    mkTriangleInExactCoordinates<dim>(top_left, bottom_right, bottom_left, distance, triangle + index);
}

template<nr_uint dim>
void fillTriangles(
    const ScreenDimension& screenDim, 
    const BinQueueConfig config,
    const nr_uint totalWorkGroupCount,
    const nr_float expectedDepth,
    const nr_uint batchSize, 
    Triangle<dim>* triangles,
    nr_uint* binQueues)
{
    const nr_uint binCountX = ceil(((nr_float) screenDim.width) / config.binWidth);
    const nr_uint binCountY = ceil(((nr_float) screenDim.height) / config.binHeight);
    const nr_uint totalBinCount = binCountX * binCountY;

    const nr_uint elementsPerGroup = totalBinCount * (config.queueSize + 1);

    for (nr_uint i = 0; i < elementsPerGroup * totalWorkGroupCount; i += config.queueSize + 1)
    {
        binQueues[i] = 1;
    }

    nr_uint i = 0;
    nr_uint g = 0;
    nr_uint binOffset = 0;
    nr_uint currentQueueBase;

    Bin bin{ config.binWidth, config.binHeight, 0, 0 };

    for (nr_uint y = 0; y < binCountY; ++y)
    {
        for (nr_uint x = 0; x < binCountX; ++x)
        {
            bin.x = x * config.binWidth;
            bin.y = y * config.binHeight;

            mkTriangleFullyInBin(screenDim, bin, pow(expectedDepth, 0.5), i, triangles);
            mkTriangleFullyInBin(screenDim, bin, expectedDepth, i + 1, triangles);
            mkTriangleFullyInBin(screenDim, bin, pow(expectedDepth, 0.3), i + 2, triangles);
            
            binOffset = (y * binCountX + x) * (config.queueSize + 1);
            currentQueueBase = g * elementsPerGroup + binOffset;

            if (g == i / batchSize)
            {
                binQueues[currentQueueBase] = 0;
                binQueues[currentQueueBase + 1] = i;
                binQueues[currentQueueBase + 2] = i + 1;
                binQueues[currentQueueBase + 3] = i + 2;
                if (config.queueSize > 3) binQueues[currentQueueBase + 4] = 0;
            }
            else
            {
                g = (i / batchSize) % totalWorkGroupCount;
            }

            i += 3;
        }
    }
}

nr::Module mkFineModule(const nr_uint dim, cl_status* err)
{
    auto opts = mkStandardOptions(dim);
    opts.push_back(TEST_FINE);
    auto ret = nr::Module({nr::__internal::clcode::base, nr::__internal::clcode::fine_rasterizer}, err);
    *err = ret.build(opts);
    return ret;
}
