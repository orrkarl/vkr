#pragma once

#include "../general/predefs.h"

namespace nr
{

const string rasterizer_generic_dim = R"__CODE__(

#define SWAP(x, y)  \
x ^= y  \
y ^= x  \
x ^= y

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

#ifdef _DEBUG

#define debug_message(fmt) printf(fmt)
#define debug_message1(fmt, arg1) printf(fmt, arg1)
#define debug_message2(fmt, arg1, arg2) printf(fmt, arg1, arg2)
#define debug_message3(fmt, arg1, arg2, arg3) printf(fmt, arg1, arg2, arg3)
#define debug_message4(fmt, arg1, arg2, arg3, arg4) printf(fmt, arg1, arg2, arg3, arg4)

#else

#define debug_message(fmt)
#define debug_message1(fmt, arg1)
#define debug_message2(fmt, arg1, arg2)
#define debug_message3(fmt, arg1, arg2, arg3)
#define debug_message4(fmt, arg1, arg2, arg3, arg4)

#endif // _DEBUG

int reportI(constant char* fmt, const int arg)
{
    debug_message1(fmt, arg);
    return arg;
}

uint reportUI(constant char* fmt, const uint arg)
{
    debug_message1(fmt, arg);
    return arg;
}

uint getPixelIndex(constant RasterizeInfo* info, const Pixel pixel)
{
    return 3 * (info->width * pixel.position.y + pixel.position.x) + pixel.color;
}

uint positionToPixel(const uint axisLength, const float axisPosition)
{
    return (uint) ((axisLength - 1) * 0.5 * (1 + axisPosition));
}

int ndcToSignedPixel(const uint axisLength, const float axisPosition)
{
    int l = (int) axisLength - 1;
    return floor(l * 0.5 * (1 + axisPosition) - (l / 2));
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

void paintPointI(
    constant RasterizeInfo* info,
    const int x,
    const int y,
    const ColorRGB color,
    global uchar* dest)
{
    Position pos;
    pos.x = (info->width - 1) / 2 + x;
    pos.y = info->height / 2 - y;
    debug_message2("painting point: (%x, %x)\n", pos.x, pos.y);
    paintPixel(info, pos, color, dest);
}

void paintPointF(
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

void paintLowLine(
    constant RasterizeInfo* info,
    const int x0,
    const int y0,
    const int x1,
    const int y1,
    const ColorRGB color,
    global uchar* dest)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    
    int yi = 1;
    if (dy < 0)
    {
        yi = -1;
        dy = -dy;
    }

    int dx2 = 2 * dx;
    int dy2 = 2 * dy;

    int d = dy2 - dx;
    int y = y0;
    
    for (int x = x0; x <= x1; ++x)
    {
        debug_message2("asking to paint: (%d, %d)\n", x, y);
        paintPointI(info, x, y, color, dest);
        if (d > 0)
        {
            y += yi;
            d -= dx2;
        }
        d += dy2;
    }
}

void paintHighLine(
    constant RasterizeInfo* info,
    const int x0,
    const int y0,
    const int x1,
    const int y1,
    const ColorRGB color,
    global uchar* dest)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    
    int xi = 1;
    if (dx < 0)
    {
        xi = -1;
        dx = -dx;
    }

    int dx2 = 2 * dx;
    int dy2 = 2 * dy;

    int d = dx2 - dy;
    int x = x0;
    
    for (int y = y0; y <= y1; ++y)
    {
        debug_message2("asking to paint: (%d, %d)\n", x, y);
        paintPointI(info, x, y, color, dest);
        if (d > 0)
        {
            x += xi;
            d -= dy2;
        }
        d += dx2;
    }
}

void paintLineBresenham(
    constant RasterizeInfo* info,
    int x0,
    int y0,
    int x1,
    int y1,
    const ColorRGB color,
    global uchar* dest)
{
    if (abs_diff(y0, y1) < abs_diff(x0, x1))
    {
        if (x0 <= x1)
        {
            printf("Option 1\n");
            paintLowLine(info, x0, y0, x1, y1, color, dest);
        }
        else
        {
            printf("Option 2\n");
            paintLowLine(info, x1, y1, x0, y0, color, dest);
        }
    }
    else
    {
        if (y0 <= y1)
        {
            printf("Option 3\n");
            paintHighLine(info, x0, y0, x1, y1, color, dest);
        }
        else
        {
            printf("Option 4\n");
            paintHighLine(info, x1, y1, x0, y0, color, dest);
        }
    }
}

void paintLine(
    constant RasterizeInfo* info, 
    const NDCLine line,
    const ColorRGB color,
    global uchar* dest)
{
    paintLineBresenham(
        info,
        reportI("start x: %d, ", ndcToSignedPixel(info->width, line.start.x)),
        reportI("start y: %d, ", ndcToSignedPixel(info->height, line.start.y)),
        reportI("end x: %d, ", ndcToSignedPixel(info->width, line.end.x)),
        reportI("end y: %d\n", ndcToSignedPixel(info->height, line.end.y)),
        color, 
        dest);
}

kernel void points(
    constant RasterizeInfo* info, 
    global const float* src, 
    global uchar* dest)
{
    const uint i = (info->dimension + 1) * get_global_id(0);
    NDCPosition point;
    point.x = src[i];
    point.y = src[i + 1];
    ColorRGB RED = { 255, 0, 0 };
    paintPointF(info, point, RED, dest);
}

kernel void lines(
    constant RasterizeInfo* info, 
    global const float* src, 
    global uchar* dest)
{
    const uint i = (2 * info->dimension + 2) * get_global_id(0);
    ColorRGB RED = { 255, 0, 0 };
    NDCLine line;
    NDCPosition start = { src[i], src[i + 1] };
    NDCPosition end   = { src[i + info->dimension + 1], src[i + info->dimension + 1 + 1] };
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

