#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace __internal
{

namespace clcode
{

const string fine_rasterizer = R"__CODE__(

bool is_bin_queue_empty(Index* queue_base)
{
    return queue_base[0];
}

kernel void fine_rasterize(
    const global Simplex* simplex_data,
    const ScreenDimension dim,
    const BinQueueConfig config,
    const uint workGroupCountX,
    const uint workGroupCountY,
    const global Index* bin_queues,
    global ColorRGB* color,
    global Depth* depth,
    global Index* index)
{
    
}

)__CODE__";

}

}

}