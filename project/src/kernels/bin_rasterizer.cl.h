/**
 * @file
 * 
 * @brief Contains the bin rasterizer stage 
 * 
 * Refer to BinRasterizer for detailed explenation of the algorithm
 * 
 * @author Orr Karl (karlor041@gmail.com)
 * @version 0.6.0
 * @date 2019-08-27
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../predefs.h"

namespace nr
{

namespace detail
{

// Main bin raster kernel name
NRAPI extern const char* BIN_RASTER_KERNEL_NAME;

namespace clcode
{

// Bin rasterizer module
NRAPI extern const char* bin_rasterizer;

}

}

}