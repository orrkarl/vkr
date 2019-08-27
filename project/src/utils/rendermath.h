/**
 * @file
 * @author Orr Karl (karlor041@gmail.com)
 * @brief Mathematical utilities for the pipeline
 * @version 0.6.0
 * @date 2019-08-27
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../predefs.h"

#include <utility>

#include "../rendering/Render.h"

namespace nr
{

namespace detail
{

/**
 * @brief Counts how many triangles are there for K simplices in N dimensional space
 * 
 * Returns K * (N choose 3)
 * @param dim space dimension
 * @param simplexCount simplices count
 * @return triangle count 
 */
NRAPI nr_uint triangleCount(const nr_uint dim, const nr_uint simplexCount);

/**
 * @brief Finds amount of raster bins of given size in given viewport
 * 
 * @param screenDim viewport size
 * @param config raster bin configuration
 * @return amount of bins in viewport
 */
NRAPI std::pair<nr_uint, nr_uint> getBinCount(const ScreenDimension& screenDim, const BinQueueConfig& config);

}

}
