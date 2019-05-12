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
    const global Simplex* simplex_data,
    const global Index* bin_queues,
    const ScreenDimension dim,
    const BinQueueConfig config,
    const uint work_group_count,
    global RawColorRGB* color,
    global Depth* depth,
    global Index* stencil)
{
    
}

)__CODE__";

}

}

}