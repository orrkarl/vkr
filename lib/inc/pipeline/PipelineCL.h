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

typedef enum _PixelColor
{
    R = 0,
    G = 1,
    B = 2
} PixelColor;

typedef struct _Position
{
    uint x;
    uint y;
} Position;

typedef struct _Line
{
    Position start;
    Position end;
} Line;

typedef struct _NDCPosition
{
    float x;
    float y;
} NDCPosition;

typedef struct _NDCLine
{
    NDCPosition start;
    NDCPosition end;
} NDCLine;

typedef struct _BresenhamLine
{
    int x1;
    int y1;
    int x2;
    int y2;
} BresenhamLine;

typedef struct _Pixel
{
    Position position;
    PixelColor color;
} Pixel;

typedef struct _ColorRGB
{
    uchar r;
    uchar g;
    uchar b;
} ColorRGB;

uint getPixelIndex(constant RasterizeInfo* info, const Pixel pixel)
{
    return 3 * (info->width * pixel.position.y + pixel.position.x) + pixel.color;
}

uint positionToPixel(const uint axisLength, const float axisPosition)
{
    return (uint) ((axisLength - 1) * 0.5 * (1 + axisPosition));
}

Position fromNDCPosition(constant RasterizeInfo* info, const NDCPosition ndcPos)
{
    Position ret;
    ret.x = positionToPixel(info->width, ndcPos.x);
    ret.y = positionToPixel(info->height, ndcPos.y);
    return ret;
}

Line fromNDCLine(constant RasterizeInfo* info, const NDCLine line)
{
    Line ret;
    ret.start = fromNDCPosition(info, line.start);
    ret.end   = fromNDCPosition(info, line.end);  
    return ret;
}

void paintPixel(
    constant RasterizeInfo* info,
    const Position position,
    const ColorRGB color,
    global uchar* dest)
{
    Pixel current;
    current.position = position;
    current.color = R;
    dest[getPixelIndex(info, current)] = color.r;
    current.color = G;
    dest[getPixelIndex(info, current)] = color.g;
    current.color = B;
    dest[getPixelIndex(info, current)] = color.b;
}

void paintPoint(
    constant RasterizeInfo* info, 
    const NDCPosition pos, 
    const ColorRGB color, 
    global uchar* dest)
{
    Position index;
    index.x = positionToPixel(info->width, pos.x);
    index.y = positionToPixel(info->height, pos.y);

    paintPixel(info, index, color, dest);
}

void paintLineBresenham(
    constant RasterizeInfo* info,
    const BresenhamLine line,
    const ColorRGB color,
    global uchar* dest)
{

}

void paintLine(
    constant RasterizeInfo* info, 
    const NDCLine line,
    const ColorRGB color,
    global uchar* dest)
{
    BresenhamLine bresenhamLine = 
    {
        (int) (info->width * line.start.x),
        (int) (info->height * line.start.y),
        (int) (info->width * line.end.x), 
        (int) (info->height * line.end.y)
    };
    paintLineBresenham(info, bresenhamLine, color, dest);
}

kernel void points(
    constant RasterizeInfo* info, 
    global const float* src, 
    global uchar* dest)
{
    const uint i = info->dimension * get_global_id(0);
    NDCPosition point;
    point.x = src[i];
    point.y = src[i + 1];
    ColorRGB RED = { 255, 0, 0 };
    paintPoint(info, point, RED, dest);
}

kernel void lines(
    constant RasterizeInfo* info, 
    global const float* src, 
    global uchar* dest)
{
    const uint i = (2 * info->dimension) * get_global_id(0);
    ColorRGB RED = { 255, 0, 0 };
    NDCLine line;
    NDCPosition start = { src[i], src[i + 1] };
    NDCPosition end   = { src[i + info->dimension], src[i + info->dimension + 1] };
    line.start = start;
    line.end = end;
    paintLine(info, line, RED, dest);
}

kernel void simplices(
    constant RasterizeInfo* info, 
    global const float* src, 
    global uchar* dest)
{

}

)__CODE__";

}

