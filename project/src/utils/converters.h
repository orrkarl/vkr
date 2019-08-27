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