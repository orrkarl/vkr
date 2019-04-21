#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace __internal
{

namespace clcode
{

const string fine_rasterizer = R"__CODE__(


kernel void fine_rasterize(
    const global Simplex* simplexData,
    const ScreenDimension dim,
    const BinQueueConfig config,
    const global Index* bin_queues,
    global uint* overflow,
    global Quad* quadQueues)
{

}

)__CODE__";

}

}

}