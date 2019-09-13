/**
 * @file
 * @author Orr Karl (karlor041@gmail.com)
 * @brief Utility file for converting useful types
 * @version 0.6.0
 * @date 2019-08-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../predefs.h"

namespace nr
{

namespace utils
{

/**
 * @brief converts an OpenCL error to it's string representation
 * 
 * @param error OpenCL error code
 * @return string error representation 
 */
NRAPI const string stringFromCLError(const cl_int& error);

}

}