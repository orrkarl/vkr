/**
 * @file
 * 
 * @brief Containing the simplex reduce device code
 * 
 * Refer to SimplexReducer for detailed explenation of the algorithm
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

// Main simplex reduce kernel name
NRAPI extern const char* SIMPLEX_REDUCE_KERNEL_NAME;

namespace clcode
{

// simplex reduce module
NRAPI extern const char* simplex_reduce;

}

}

}
