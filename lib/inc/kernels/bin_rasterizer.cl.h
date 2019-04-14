#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace __internal
{

namespace clcode
{

const string bin_rasterizer = R"__CODE__(

#define BATCH_SIZE (256)

typedef struct _Bin
{
    uint width;
    uint height;
    uint x;
    uint y;
} Bin;

bool is_point_in_bin(const Point point, const Bin bin)
{
    return bin.x <= point[0] && point[0] <= bin.x + bin.width && bin.y <= point[1] && point[1] <= bin.y + bin.height; 
}

bool is_simplex_in_bin(const Simplex simplex, const Bin bin)
{
    bool ret = false;

    __attribute__((opencl_unroll_hint))         // hopefully the compiler will unroll this loop
    for (uint i = 0; i < RenderDimension; ++i)
    {
        ret |= is_point_in_bin(simplex[i], bin);
    }

    return ret;
}

kernel void bin_rasterize(
    constant Simplex* simplexData,
    uint simplexCount,
    ScreenDimension dim,
    uint binWidth,
    uint binHeight,
    uint binQueueSize,
    global Index* binQueues)
{
    const uint group = get_group_id(0);
    const uint target_bin_x = get_local_id(0);
    const uint target_bin_y = get_local_id(1); 

}


)__CODE__";

}

}

}