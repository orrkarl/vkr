#pragma once

#include <inc/includes.h>
#include <inc/modules/binning/bin_utils.h>
#include <rendering/Render.h>
#include <pipeline/BinRasterizer.h>

const RawColorRGB RED = { 255, 0, 0 };

template<NRuint dim>
void mkTriangleInCoordinates(const NDCPosition p0, const NDCPosition p1, const NDCPosition p2, Triangle<dim>* triangle)
{
    mkTriangleInCoordinates(p0, p1, p2, 0, triangle);
}

template<NRuint dim>
void mkTriangleInCoordinates(const NDCPosition p0, const NDCPosition p1, const NDCPosition p2, const NRfloat minDistance, Triangle<dim>* triangle)
{
    mkTriangleInExactCoordinates(p0, p1, p2, minDistance + (1 - minDistance) * rand() / RAND_MAX, triangle);
}

template<NRuint dim>
void mkTriangleInExactCoordinates(const NDCPosition p0, const NDCPosition p1, const NDCPosition p2, const NRfloat distance, Triangle<dim>* triangle)
{
    triangle->points[0].values[0] = p0.x;
    triangle->points[0].values[1] = p0.y;

    triangle->points[1].values[0] = p1.x;
    triangle->points[1].values[1] = p1.y;
    
    triangle->points[2].values[0] = p2.x;
    triangle->points[2].values[1] = p2.y;

    for (NRuint i = 0; i < 3; ++i)
    {
        for (NRuint j = 2; j < dim; ++j)
        {
            triangle->points[i].values[j] = distance;
        }
    }
}

template<NRuint dim>
void mkTriangleFullyInBin(const ScreenDimension& screenDim, const Bin& bin, const NRfloat distance, const NRuint index, Triangle<dim>* triangle)
{
    ScreenPosition top_left_screen     = { bin.x, bin.y + 3 * bin.height / 4 };
    ScreenPosition bottom_left_screen  = { bin.x, bin.y + bin.height / 4 };
    ScreenPosition bottom_right_screen = { bin.x + bin.width / 2, bin.y + bin.height / 4 };

    NDCPosition top_left     = ndcFromScreen(top_left_screen, screenDim);
    NDCPosition bottom_left  = ndcFromScreen(bottom_left_screen, screenDim);
    NDCPosition bottom_right = ndcFromScreen(bottom_right_screen, screenDim);

    mkTriangleInExactCoordinates<dim>(top_left, bottom_right, bottom_left, distance, triangle + index);
}

template<NRuint dim>
void fillTriangles(
    const ScreenDimension& screenDim, 
    const BinQueueConfig config,
    const NRuint totalWorkGroupCount,
    const NRfloat expectedDepth,
    const NRuint batchSize, 
    Triangle<dim>* triangles,
    NRuint* binQueues)
{
    const NRuint binCountX = ceil(((NRfloat) screenDim.width) / config.binWidth);
    const NRuint binCountY = ceil(((NRfloat) screenDim.height) / config.binHeight);
    const NRuint totalBinCount = binCountX * binCountY;

    const NRuint elementsPerGroup = totalBinCount * (config.queueSize + 1);

    for (NRuint i = 0; i < elementsPerGroup * totalWorkGroupCount; i += config.queueSize + 1)
    {
        binQueues[i] = 1;
    }

    NRuint i = 0;
    NRuint g = 0;
    NRuint binOffset = 0;
    NRuint currentQueueBase;

    Bin bin{ config.binWidth, config.binHeight, 0, 0 };

    for (NRuint y = 0; y < binCountY; ++y)
    {
        for (NRuint x = 0; x < binCountX; ++x)
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
