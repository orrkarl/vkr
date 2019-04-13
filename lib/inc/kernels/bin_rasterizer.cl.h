#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace __internal
{

namespace clcode
{

const string bin_rasterizer = R"__CODE__(

kernel void bin_rasterize(
    constant Simplex* simplexData,
    uint simplexCount,
    ScreenDimension dim,
    uint binWidth,
    uint binHeight,
    uint binQueueSize,
    global Index* binQueues)
{
    
}


)__CODE__";

}

}

}