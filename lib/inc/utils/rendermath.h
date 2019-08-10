#pragma once

#include "../general/predefs.h"

#include <cmath>

namespace nr
{

namespace detail
{

nr_uint triangleCount(const nr_uint dim, const nr_uint simplexCount)
{
	return dim * (dim - 1) * (dim - 2) * simplexCount;
}

}

}
