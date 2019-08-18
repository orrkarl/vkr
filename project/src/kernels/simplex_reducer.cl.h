#pragma once

#include "../predefs.h"

namespace nr
{

namespace detail
{

NRAPI extern const char* SIMPLEX_REDUCE_KERNEL_NAME;

namespace clcode
{

// Reducing simplexes to their triangles
NRAPI extern const char* simplex_reduce;

}

}

}
