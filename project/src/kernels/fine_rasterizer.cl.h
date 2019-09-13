/**
 * @file
 * 
 * @brief Contains the fine rasterizer stage  
 * 
 * Refer to FineRasterizer for detailed explenation of the algorithm
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

// Main fine raster kernel name
NRAPI extern const char* FINE_RASTER_KERNEL_NAME;

namespace clcode
{

// fine rasterizer module
NRAPI extern const char* fine_rasterizer;

}

}

}