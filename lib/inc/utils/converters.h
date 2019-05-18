#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace utils
{
    
NR_SHARED_EXPORT Error fromCLError(const cl_int& err);

NR_SHARED_EXPORT const string stringFromCLError(const cl_int& error);

NR_SHARED_EXPORT const string stringFromNRError(const Error& err);

}

}