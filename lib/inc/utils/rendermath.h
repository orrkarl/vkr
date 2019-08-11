#pragma once

#include "../general/predefs.h"

#include <utility>

#include "../rendering/Render.h"

namespace nr
{

namespace detail
{

nr_uint triangleCount(const nr_uint dim, const nr_uint simplexCount);

std::pair<nr_uint, nr_uint> getBinCount(const ScreenDimension& screenDim, const BinQueueConfig& config);

}

}
