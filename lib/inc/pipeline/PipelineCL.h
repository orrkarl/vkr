#pragma once

#include "../general/predefs.h"

namespace nr
{

const string rasterizer_generic_dim = R"__CODE__(
typedef struct _RasterizeInfo
{
    const uint dimension;
    const uint width, height;
} RasterizeInfo;

const uint R = 0;
const uint G = 1;
const uint B = 2;

uint getPixelIndex(constant RasterizeInfo* info, const uint x, const uint y, const uint pixel)
{
    return 3 * (info->width * y + x) + pixel;
}

void ndcToScreen(constant RasterizeInfo* info, const float x, const float y, global uchar* dest)
{
    uint index_x = (uint) ((info->width - 1) * 0.5 * (1 + x));
    uint index_y = (uint) ((info->height - 1) * 0.5 * (1 + y));
    
    dest[getPixelIndex(info, index_x, index_y, R)] = 255;
    dest[getPixelIndex(info, index_x, index_y, G)] = 0;
    dest[getPixelIndex(info, index_x, index_y, B)] = 0;
}

kernel void rasterize(constant RasterizeInfo* info, global const float* src, global uchar* dest)
{
    const uint i = 2 * get_global_id(0);
    ndcToScreen(info, src[i], src[i + 1], dest);
}

)__CODE__";

}

