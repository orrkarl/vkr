#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace __internal
{

namespace clcode
{

const string base = R"__CODE__(

// -------------------------------------- Types -------------------------------------- 

// #define RENDER_DIMENSION 4

#ifndef RENDER_DIMENSION 
    #error "RENDER_DIMENSION has to be defined!"
#endif

#if RENDER_DIMENSION < 3
    #error "RENDER_DIMENSION has to be at least 3!"
#endif

typedef struct _ScreenDimension
{
    uint width;
    uint height;
} ScreenDimension;

typedef uint2 ScreenPosition;

typedef int2 SignedScreenPosition;

typedef float2 NDCPosition;

typedef float3 ColorRGB;

typedef float4 ColorRGBA;

typedef uint  Index;
typedef float Depth;

typedef struct _FrameBuffer
{
    ColorRGB*       color;
    Index*          stencil;
    Depth*          depth;
} FrameBuffer;

typedef struct _Fragment 
{
    ColorRGB        color;
    ScreenPosition  position;
    Depth           depth;
    Index           primitive;
} Fragment;

typedef float Point[RENDER_DIMENSION];   // point in n-dimensional space 
typedef Point Simplex[RENDER_DIMENSION]; // N-1 simplex (rendering is done on an object's surface)

// ----------------------------------------------------------------------------

// -------------------------------------- Utilities -------------------------------------- 

uint index_from_screen(const ScreenPosition pos, const ScreenDimension dim)
{
    return pos.y * dim.width + pos.x;
}

uint from_continuous(const float continuous)
{
    return floor(continuous);
}

uint axis_screen_from_ndc(const float pos, const uint length)
{
    return from_continuous((pos + 1) * (length - 1) / 2);
}

void screen_from_ndc(const NDCPosition ndc, const ScreenDimension dim, ScreenPosition* screen)
{
    screen->x = axis_screen_from_ndc(ndc.x, dim.width);
    screen->y = axis_screen_from_ndc(ndc.y, dim.height);
}

int axis_signed_from_ndc(const float pos, const uint length)
{
    return axis_screen_from_ndc(pos, length) - length / 2;
}

void signed_from_ndc(const NDCPosition ndc, const ScreenDimension dim, SignedScreenPosition* screen)
{
    screen->x = axis_signed_from_ndc(ndc.x, dim.width);
    screen->y = axis_signed_from_ndc(ndc.y, dim.height);
}

void screen_from_signed(const SignedScreenPosition pos, const ScreenDimension dim, ScreenPosition* screen)
{
    screen->x = pos.x + dim.width / 2;
    screen->y = pos.y + dim.height / 2;
}

// ----------------------------------------------------------------------------

// -------------------------------------- Draw -------------------------------------- 

void apply_fragment_full(const Fragment fragment, const ScreenDimension dim, FrameBuffer* frame)
{ 
    frame->color[index_from_screen(fragment.position, dim)] = fragment.color;
}

// ----------------------------------------------------------------------------

// -------------------------------------- Testing -------------------------------------- 

kernel void screen_from_ndc_kernel(NDCPosition pos, ScreenDimension dim, global ScreenPosition* res)
{
    ScreenPosition tmp;
    screen_from_ndc(pos, dim, &tmp);
    res->x = tmp.x;
    res->y = tmp.y;
}

)__CODE__";

}

}

}