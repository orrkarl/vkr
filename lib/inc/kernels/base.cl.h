#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace detail
{

namespace clcode
{

// All of the cross-file utilities and types used in my cl code are here
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

#define ELEMENTS_PER_POINT (RENDER_DIMENSION + 1)

typedef struct _ScreenDimension
{
    uint width;
    uint height;
} ScreenDimension;

typedef struct _ScreenPosition
{
	uint x, y;
} ScreenPosition;

typedef struct _SignedScreenPosition
{
	int x, y;
} SignedScreenPosition;

typedef struct _NDCPosition
{
	float x, y;
} NDCPosition;

typedef struct _ColorRGB
{
	float r, g, b;
} ColorRGB;

typedef struct _ColorRGBA
{
	float r, g, b, a;
} ColorRGBA;

typedef uint  Index;
typedef float Depth;

typedef struct _RawColorRGBA
{
    uchar r, g, b, a;
} RawColorRGBA;

typedef struct _FrameBuffer
{
    RawColorRGBA*    color;
    Depth*          depth;
} FrameBuffer;

typedef struct _Fragment
{
    ScreenPosition position;
    RawColorRGBA color;
    Depth depth;
} Fragment;

typedef struct _Bin
{
    uint width;
    uint height;
    uint x;
    uint y;
} Bin;

typedef float Point[ELEMENTS_PER_POINT]; // point in n-dimensional space 
typedef Point Triangle[3];               // Nth dimensional triangle
typedef Point Simplex[RENDER_DIMENSION]; // N-1 simplex (rendering is done on an object's surface)

typedef struct _BinQueueConfig
{
    uint bin_width;
    uint bin_height;
    uint queue_size;
} BinQueueConfig;

// ----------------------------------------------------------------------------

// -------------------------------------- Globals -------------------------------------- 

#define RAW_RED ((RawColorRGBA) {255, 0, 0})

#define r(vec) (vec.x)
#define g(vec) (vec.y)
#define b(vec) (vec.z)

// ----------------------------------------------------------------------------


// -------------------------------------- Utilities -------------------------------------- 

// Returns the 1d index of pos in a 2d array with width and height as in dim
uint index_from_screen(const ScreenPosition pos, const ScreenDimension dim)
{
    return pos.y * dim.width + pos.x;
}

// Deprecated
uint from_continuous(const float continuous)
{
    return floor(continuous);
}

// Deprecated
float from_discrete(const uint discrete)
{
    return discrete + 0.5;
}

// Convertes NDC to Screen Coordinates for one axis
uint axis_screen_from_ndc(const float pos, const uint length)
{
    return (pos + 1) * length * 0.5;
}

// Convertes Screen Coordinates to NDC for one axis
float axis_ndc_from_screen(const uint pos, const uint length)
{
    return pos * 2.0 / length - 1;
}

// Convertes the middle of a pixel to NDC, for one axis
float axis_pixel_mid_point(const uint pos, const uint length)
{
    return (pos + 0.5) * 2.0 / length - 1.0;
}


// Deprecated
int axis_signed_from_ndc(const float pos, const uint length)
{
    return axis_screen_from_ndc(pos, length) - length / 2;
}

// Convertes NDC to Screen Coordinates
void screen_from_ndc(const NDCPosition ndc, const ScreenDimension dim, global ScreenPosition* screen)
{
    screen->x = axis_screen_from_ndc(ndc.x, dim.width);
    screen->y = axis_screen_from_ndc(ndc.y, dim.height);
}

// Convertes Screen Coordinates to NDC
void ndc_from_screen(const ScreenPosition screen, const ScreenDimension dim, global NDCPosition* result)
{
    result->x = axis_ndc_from_screen(screen.x, dim.width);
    result->y = axis_ndc_from_screen(screen.y, dim.height);
}

void ndc_from_screen_p(const ScreenPosition screen, const ScreenDimension dim, NDCPosition* result)
{
    result->x = axis_ndc_from_screen(screen.x, dim.width);
    result->y = axis_ndc_from_screen(screen.y, dim.height);
}


// Convertes the middle of a pixel to NDC
void pixel_mid_point_from_screen(const ScreenPosition screen, const ScreenDimension dim, NDCPosition* result)
{
    result->x = axis_pixel_mid_point(screen.x, dim.width);
    result->y = axis_pixel_mid_point(screen.y, dim.height);
}

// Deprecated
void signed_from_ndc(const NDCPosition ndc, const ScreenDimension dim, SignedScreenPosition* screen)
{
    screen->x = axis_signed_from_ndc(ndc.x, dim.width);
    screen->y = axis_signed_from_ndc(ndc.y, dim.height);
}

// Deprecated
void screen_from_signed(const SignedScreenPosition pos, const ScreenDimension dim, global ScreenPosition* screen)
{
    screen->x = pos.x + dim.width / 2;
    screen->y = pos.y + dim.height / 2;
}


// ----------------------------------------------------------------------------

// -------------------------------------- Debugging -------------------------------------- 

#ifdef _DEBUG
    #define DEBUG(code) code
#else
    #define DEBUG(code)
#endif // _DEBUG

#define IS_WORK_ITEM_GLOBAL(i, j, k) (get_global_id(0) == i && get_global_id(1) == j && get_global_id(2) == k)
#define IS_WORK_ITEM_LOCAL(i, j, k) (get_local_id(0) == i && get_local_id(1) == j && get_local_id(2) == k)

#define IS_GROUP_HEAD  IS_WORK_ITEM_LOCAL(0, 0, 0)
#define IS_GLOBAL_HEAD IS_WORK_ITEM_GLOBAL(0, 0, 0)

#define DEBUG_MESSAGE(msg)                                                          DEBUG(printf(msg))
#define DEBUG_MESSAGE1(msg, arg1)                                                   DEBUG(printf(msg, arg1))
#define DEBUG_MESSAGE2(msg, arg1, arg2)                                             DEBUG(printf(msg, arg1, arg2))
#define DEBUG_MESSAGE3(msg, arg1, arg2, arg3)                                       DEBUG(printf(msg, arg1, arg2, arg3))
#define DEBUG_MESSAGE4(msg, arg1, arg2, arg3, arg4)                                 DEBUG(printf(msg, arg1, arg2, arg3, arg4))
#define DEBUG_MESSAGE5(msg, arg1, arg2, arg3, arg4, arg5)                           DEBUG(printf(msg, arg1, arg2, arg3, arg4, arg5))
#define DEBUG_MESSAGE6(msg, arg1, arg2, arg3, arg4, arg5, arg6)                     DEBUG(printf(msg, arg1, arg2, arg3, arg4, arg5, arg6))
#define DEBUG_MESSAGE7(msg, arg1, arg2, arg3, arg4, arg5, arg6, arg7)               DEBUG(printf(msg, arg1, arg2, arg3, arg4, arg5, arg6, arg7))
#define DEBUG_MESSAGE8(msg, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)         DEBUG(printf(msg, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8))
#define DEBUG_MESSAGE9(msg, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9)   DEBUG(printf(msg, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9))

#define DEBUG_ITEM_SPECIFIC(i, j, k, msg) DEBUG(if (IS_WORK_ITEM_GLOBAL(i, j, k)) { printf(msg); } else {})
#define DEBUG_ITEM_SPECIFIC1(i, j, k, msg, arg1) DEBUG(if (IS_WORK_ITEM_GLOBAL(i, j, k)) { printf(msg, arg1); } else {})
#define DEBUG_ITEM_SPECIFIC2(i, j, k, msg, arg1, arg2) DEBUG(if (IS_WORK_ITEM_GLOBAL(i, j, k)) { printf(msg, arg1, arg2); } else {})
#define DEBUG_ITEM_SPECIFIC3(i, j, k, msg, arg1, arg2, arg3) DEBUG(if (IS_WORK_ITEM_GLOBAL(i, j, k)) { printf(msg, arg1, arg2, arg3); } else {})
#define DEBUG_ITEM_SPECIFIC4(i, j, k, msg, arg1, arg2, arg3, arg4) DEBUG(if (IS_WORK_ITEM_GLOBAL(i, j, k)) { printf(msg, arg1, arg2, arg3, arg4); } else {})
#define DEBUG_ITEM_SPECIFIC5(i, j, k, msg, arg1, arg2, arg3, arg4, arg5) DEBUG(if (IS_WORK_ITEM_GLOBAL(i, j, k)) { printf(msg, arg1, arg2, arg3, arg4, arg5); } else {})
#define DEBUG_ITEM_SPECIFIC6(i, j, k, msg, arg1, arg2, arg3, arg4, arg5, arg6) DEBUG(if (IS_WORK_ITEM_GLOBAL(i, j, k)) { printf(msg, arg1, arg2, arg3, arg4, arg5, arg6); } else {})
#define DEBUG_ITEM_SPECIFIC7(i, j, k, msg, arg1, arg2, arg3, arg4, arg5, arg6, arg7) DEBUG(if (IS_WORK_ITEM_GLOBAL(i, j, k)) { printf(msg, arg1, arg2, arg3, arg4, arg5, arg6, arg7); } else {})
#define DEBUG_ITEM_SPECIFIC8(i, j, k, msg, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) DEBUG(if (IS_WORK_ITEM_GLOBAL(i, j, k)) { printf(msg, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8); } else {})

#define DEBUG_LOCAL_ITEM_SPECIFIC(i, j, k, msg) DEBUG(if (IS_WORK_ITEM_LOCAL(i, j, k)) { printf(msg); } else {})
#define DEBUG_LOCAL_ITEM_SPECIFIC1(i, j, k, msg, arg1) DEBUG(if (IS_WORK_ITEM_LOCAL(i, j, k)) { printf(msg, arg1); } else {})
#define DEBUG_LOCAL_ITEM_SPECIFIC2(i, j, k, msg, arg1, arg2) DEBUG(if (IS_WORK_ITEM_LOCAL(i, j, k)) { printf(msg, arg1, arg2); } else {})
#define DEBUG_LOCAL_ITEM_SPECIFIC3(i, j, k, msg, arg1, arg2, arg3) DEBUG(if (IS_WORK_ITEM_LOCAL(i, j, k)) { printf(msg, arg1, arg2, arg3); } else {})
#define DEBUG_LOCAL_ITEM_SPECIFIC4(i, j, k, msg, arg1, arg2, arg3, arg4) DEBUG(if (IS_WORK_ITEM_LOCAL(i, j, k)) { printf(msg, arg1, arg2, arg3, arg4); } else {})
#define DEBUG_LOCAL_ITEM_SPECIFIC5(i, j, k, msg, arg1, arg2, arg3, arg4, arg5) DEBUG(if (IS_WORK_ITEM_LOCAL(i, j, k)) { printf(msg, arg1, arg2, arg3, arg4, arg5); } else {})
#define DEBUG_LOCAL_ITEM_SPECIFIC6(i, j, k, msg, arg1, arg2, arg3, arg4, arg5, arg6) DEBUG(if (IS_WORK_ITEM_LOCAL(i, j, k)) { printf(msg, arg1, arg2, arg3, arg4, arg5, arg6); } else {})
#define DEBUG_LOCAL_ITEM_SPECIFIC7(i, j, k, msg, arg1, arg2, arg3, arg4, arg5, arg6, arg7) DEBUG(if (IS_WORK_ITEM_LOCAL(i, j, k)) { printf(msg, arg1, arg2, arg3, arg4, arg5, arg6, arg7); } else {})
#define DEBUG_LOCAL_ITEM_SPECIFIC8(i, j, k, msg, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) DEBUG(if (IS_WORK_ITEM_LOCAL(i, j, k)) { printf(msg, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8); } else {})

// Prints only from the globally first work item
#define DEBUG_ONCE(msg)                                             DEBUG(if (IS_GLOBAL_HEAD) { printf(msg); } else {})
#define DEBUG_ONCE1(msg, arg1)                                      DEBUG(if (IS_GLOBAL_HEAD) { printf(msg, arg1); } else {}) 
#define DEBUG_ONCE2(msg, arg1, arg2)                                DEBUG(if (IS_GLOBAL_HEAD) { printf(msg, arg1, arg2); } else {})
#define DEBUG_ONCE3(msg, arg1, arg2, arg3)                          DEBUG(if (IS_GLOBAL_HEAD) { printf(msg, arg1, arg2, arg3); } else {})
#define DEBUG_ONCE4(msg, arg1, arg2, arg3, arg4)                    DEBUG(if (IS_GLOBAL_HEAD) { printf(msg, arg1, arg2, arg3, arg4); } else {})
#define DEBUG_ONCE5(msg, arg1, arg2, arg3, arg4, arg5)              DEBUG(if (IS_GLOBAL_HEAD) { printf(msg, arg1, arg2, arg3, arg4, arg5); } else {})
#define DEBUG_ONCE6(msg, arg1, arg2, arg3, arg4, arg5, arg6)        DEBUG(if (IS_GLOBAL_HEAD) { printf(msg, arg1, arg2, arg3, arg4, arg5, arg6); } else {})
#define DEBUG_ONCE7(msg, arg1, arg2, arg3, arg4, arg5, arg6, arg7)  DEBUG(if (IS_GLOBAL_HEAD) { printf(msg, arg1, arg2, arg3, arg4, arg5, arg6, arg7); } else {})
#define DEBUG_ONCE8(msg, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)  DEBUG(if (IS_GLOBAL_HEAD) { printf(msg, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8); } else {})

// ----------------------------------------------------------------------------

// -------------------------------------- Testing -------------------------------------- 

// Unit testing screen_from_ndc
kernel void screen_from_ndc_test(NDCPosition pos, ScreenDimension dim, global ScreenPosition* res)
{
    screen_from_ndc(pos, dim, res);
}

// Unit testing ndc_from_screen
kernel void ndc_from_screen_test(ScreenPosition pos, ScreenDimension dim, global NDCPosition* res)
{
    ndc_from_screen(pos, dim, res);
}

)__CODE__";

}

}

}