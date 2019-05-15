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

#ifndef RENDER_DIMENSION 
    #error "RENDER_DIMENSION has to be defined!"
#endif

#if RENDER_DIMENSION < 3
    #error "RENDER_DIMENSION has to be at least 3!"
#endif

#ifndef MAX_WORK_GROUP_COUNT
    #define MAX_WORK_GROUP_COUNT (16)
#endif

typedef struct _ScreenDimension
{
    uint width;
    uint height;
} ScreenDimension;

typedef uint2 ScreenPosition;

typedef int2 SignedScreenPosition;

typedef float2 NDCPosition;

typedef uchar3 RawColorRGB;

typedef float3 ColorRGB;

typedef float4 ColorRGBA;

typedef uint  Index;
typedef float Depth;

typedef struct _FrameBuffer
{
    RawColorRGB*    color;
    Index*          stencil;
    Depth*          depth;
} FrameBuffer;

typedef struct _Fragment
{
    ScreenPosition position;
    RawColorRGB color;
    Index stencil;
    Depth depth;
} Fragment;

typedef struct _Bin
{
    uint width;
    uint height;
    uint x;
    uint y;
} Bin;

typedef float3 Triangle[3];               // Nth dimensional triangle
typedef float Point[RENDER_DIMENSION];   // point in n-dimensional space
typedef Point Simplex[RENDER_DIMENSION]; // N-1 simplex (rendering is done on an object's surface)

typedef struct _BinQueueConfig
{
    uint bin_width;
    uint bin_height;
    uint queue_size;
} BinQueueConfig;

// ----------------------------------------------------------------------------

// -------------------------------------- Globals -------------------------------------- 

#define RAW_RED (255, 0, 0)

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

float from_discrete(const uint discrete)
{
    return discrete + 0.5;
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

float axis_ndc_from_screen(const uint pos, const uint length)
{
    return from_discrete(pos) * 2 / (length - 1) - 1;
}

void ndc_from_screen(const ScreenPosition screen, const ScreenDimension dim, NDCPosition* result)
{
    result->x = axis_ndc_from_screen(screen.x, dim.width);
    result->y = axis_ndc_from_screen(screen.y, dim.height);
}

// ----------------------------------------------------------------------------

// -------------------------------------- Debugging -------------------------------------- 

#ifdef _DEBUG
    #define DEBUG(code) code
#else
    #define DEBUG(code)
#endif // _DEBUG

#define IS_GROUP_HEAD (!get_local_id(0) && !get_local_id(1) && !get_local_id(2))

#define DEBUG_MESSAGE(msg)                                  DEBUG(printf(msg))
#define DEBUG_MESSAGE1(msg, arg1)                           DEBUG(printf(msg, arg1))
#define DEBUG_MESSAGE2(msg, arg1, arg2)                     DEBUG(printf(msg, arg1, arg2))
#define DEBUG_MESSAGE3(msg, arg1, arg2, arg3)               DEBUG(printf(msg, arg1, arg2, arg3))
#define DEBUG_MESSAGE4(msg, arg1, arg2, arg3, arg4)         DEBUG(printf(msg, arg1, arg2, arg3, arg4))
#define DEBUG_MESSAGE5(msg, arg1, arg2, arg3, arg4, arg5)   DEBUG(printf(msg, arg1, arg2, arg3, arg4, arg5))

// Prints only from the first work item in a work group
// Prints once PER GROUP
#define DEBUG_ONCE(msg)                                 DEBUG(if (IS_GROUP_HEAD) { printf(msg); } else {})
#define DEBUG_ONCE1(msg, arg1)                          DEBUG(if (IS_GROUP_HEAD) { printf(msg, arg1); } else {}) 
#define DEBUG_ONCE2(msg, arg1, arg2)                    DEBUG(if (IS_GROUP_HEAD) { printf(msg, arg1, arg2); } else {})
#define DEBUG_ONCE3(msg, arg1, arg2, arg3)              DEBUG(if (IS_GROUP_HEAD) { printf(msg, arg1, arg2, arg3); } else {})
#define DEBUG_ONCE4(msg, arg1, arg2, arg3, arg4)        DEBUG(if (IS_GROUP_HEAD) { printf(msg, arg1, arg2, arg3, arg4); } else {})
#define DEBUG_ONCE5(msg, arg1, arg2, arg3, arg4, arg5)  DEBUG(if (IS_GROUP_HEAD) { printf(msg, arg1, arg2, arg3, arg4, arg5); } else {})

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