/**
 * @file
 *
 * @brief Contains the triangle setup stage
 *
 * Refer to TriangleSetup for detailed explenation of the algorithm
 *
 * @author Orr Karl (karlor041@gmail.com)
 * @version 0.7.0
 * @date 2019-09-23
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
NRAPI extern const char* TRIANGLE_SETUP_KERNEL_NAME;

namespace clcode
{

// fine rasterizer module
NRAPI extern const char* vertex_post_processor;

}

}

}