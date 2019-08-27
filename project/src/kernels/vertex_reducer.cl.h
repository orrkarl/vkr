/**
 * @file
 * 
 * @brief Contains the vertex reduce NR device code
 * 
 * Refer to VertexReducer for a detailed description of the algorithm 
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

// Main vertex reduce kernel name
NRAPI extern const char* VERTEX_REDUCE_KERNEL_NAME;

namespace clcode
{

// vertex reduce module
NRAPI extern const char* vertex_reduce;

}

}

}