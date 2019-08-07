#pragma once

#include <inc/includes.h>
#include <inc/modules/binning/bin_utils.h>
#include <rendering/Render.h>
#include <pipeline/BinRasterizer.h>

const RawColorRGBA RED = { 255, 0, 0 };
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
        for (nr_uint j = 2; j < dim + 1; ++j)
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

nr_float slope(const NDCPosition& p0, const NDCPosition& p1)
{
	return (p1.y - p0.y) / (p1.x - p0.x);
}

template <nr_uint dim>
void tesselateBin(
	const ScreenDimension& screenDim,
	const Bin bin,
	const nr_uint queueSize,
	const nr_uint workGroup,
	const nr_float expectedDepth,
	const bool halfBin,
	const nr_uint index,
	Triangle<dim>* triangles,
	nr_uint* binQueues,
	nr::RawColorRGBA* expectedColorBuffer,
	nr_float* expectedDepthBuffer)
{
	const nr_uint binCountY = nr_uint(ceil(nr_float(screenDim.height) / bin.height));
	const nr_uint binCountX = nr_uint(ceil(nr_float(screenDim.width) / bin.width));
	const nr_uint indexX = bin.x / bin.width;
	const nr_uint indexY = bin.y / bin.height;
	const nr_uint totalBinCount = binCountX * binCountY;
	const nr_uint binOffset = binCountX * indexY + indexX;

	Triangle<dim>* currentTriangle = triangles + index;
	nr_uint* currentQueue = binQueues + (totalBinCount * workGroup + binOffset) * (queueSize + 1);

	const auto rightEdge = std::min(bin.x + bin.width, screenDim.width);
	const auto topEdge = std::min(bin.y + bin.height, screenDim.height);

	auto screenTopLeft		= ScreenPosition{ bin.x, topEdge };
	auto screenTopRight		= ScreenPosition{ rightEdge, topEdge };
	auto screenBottomRight	= ScreenPosition{ rightEdge, bin.y };
	auto screenBottomLeft	= ScreenPosition{ bin.x, bin.y };

	auto ndcTopLeft		= ndcFromScreen(screenTopLeft, screenDim);
	auto ndcTopRight	= ndcFromScreen(screenTopRight, screenDim);
	auto ndcBottomRight = ndcFromScreen(screenBottomRight, screenDim);
	auto ndcBottomLeft	= ndcFromScreen(screenBottomLeft, screenDim);

	if (halfBin)
	{
		mkTriangleInExactCoordinates(ndcBottomLeft, ndcTopLeft, ndcTopRight, expectedDepth, currentTriangle);

		currentQueue[0] = 0;
		currentQueue[1] = index;
		currentQueue[2] = 0;

		const nr_float binSlope = slope(ndcFromPixelMid(screenTopRight, screenDim), ndcBottomLeft);

		for (auto y = bin.y; y < topEdge; ++y)
		{
			for (auto x = bin.x; x < rightEdge; ++x)
			{
				auto midpoint = ndcFromPixelMid({ x, y }, screenDim);
				auto tmpSlope = slope(midpoint, ndcBottomLeft);
				if (tmpSlope > binSlope)
				{
					auto idx = y * screenDim.width + x;
					expectedColorBuffer[idx] = RED;
					expectedDepthBuffer[idx] = 1 / expectedDepth;
				}
			}
		}

	}
	else
	{
		mkTriangleInExactCoordinates(ndcBottomLeft, ndcTopLeft, ndcTopRight, expectedDepth, currentTriangle);
		mkTriangleInExactCoordinates(ndcTopRight, ndcBottomRight, ndcBottomLeft, expectedDepth, currentTriangle + 1);

		currentQueue[0] = 0;
		currentQueue[1] = index;
		currentQueue[2] = index + 1;
		currentQueue[3] = 0;

		for (auto y = bin.y; y < topEdge; ++y)
		{
			for (auto x = bin.x; x < rightEdge; ++x)
			{
				auto idx = y * screenDim.width + x;
				expectedColorBuffer[idx] = RED;
				expectedDepthBuffer[idx] = 1 / expectedDepth;
			}
		}
	}
}

template<nr_uint dim>
void tesselateScreen(
	const ScreenDimension& screenDim,
	const BinQueueConfig config,
	const nr_uint totalWorkGroupCount,
	const nr_float expectedDepth,
	const nr_uint triangleCount,
	Triangle<dim>* triangles,
	nr_uint* binQueues,
	nr::RawColorRGBA* expectedColorBuffer,
	nr_float* expectedDepthBuffer)
{
	assert(triangleCount != 0);
	assert(triangles != nullptr);
	assert(binQueues != nullptr);
	assert(expectedColorBuffer != nullptr);
	assert(expectedDepthBuffer != nullptr);

	const nr_uint totalBinCount = nr_uint(ceil(nr_float(screenDim.width) / config.binWidth)) * nr_uint(ceil(nr_float(screenDim.height) / config.binHeight));
	assert(triangleCount < 2 * totalBinCount);

	for (auto i = 0u; i < totalBinCount * totalWorkGroupCount; ++i)
	{
		binQueues[i * (config.queueSize + 1)] = 1;
	}


	auto currentWorkGroup = 0u;
	Bin currentBin{ config.binWidth, config.binHeight, 0, 0 };
	
	for (auto currentTriangle = 0; currentTriangle < triangleCount; currentTriangle += 2)
	{
		tesselateBin(screenDim, currentBin, config.queueSize, currentWorkGroup, expectedDepth, (triangleCount - currentTriangle) == 1, currentTriangle, triangles, binQueues, expectedColorBuffer, expectedDepthBuffer);

		if (currentBin.x + currentBin.width < screenDim.width)
		{
			currentBin.x += currentBin.width;
		}
		else
		{
			currentBin.x = 0;
			currentBin.y += currentBin.height;
		}

		currentWorkGroup = (currentWorkGroup + 1) % totalWorkGroupCount;
	}
}

nr::Module mkFineModule(const nr_uint dim, cl_status* err)
{
    auto opts = mkStandardOptions(dim);
    opts.push_back(TEST_FINE);
    auto ret = nr::Module(defaultContext, Module::Sources{nr::detail::clcode::base, nr::detail::clcode::fine_rasterizer}, err);
    *err = ret.build(defaultDevice, opts);
    return ret;
}

testing::AssertionResult validateDepth(const nr_float* depthBuffer, const ScreenDimension& screenDim, const nr_float defaultDepth, const nr_float expectedDepth)
{
	for (nr_uint y = 0; y < screenDim.height; ++y)
	{
		for (nr_uint x = 0; x < screenDim.width; ++x)
		{
			nr_float actualDepth = depthBuffer[y * screenDim.width + x];
			nr_float expected = 1 / expectedDepth;
			if (std::abs(actualDepth - defaultDepth) > 10e-5 && std::abs(actualDepth - expected) > 10e-5)
			{
				return testing::AssertionFailure() << "At: (" << x << ", " << y << "). default = " << defaultDepth << ", expected = " << expected << ", actual = " << actualDepth;
			}
		}
	}

	return testing::AssertionSuccess();
}
