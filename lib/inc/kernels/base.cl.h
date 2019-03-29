#pragma once

#include "inc/general/predefs.h"

namespace nr
{

namespace __internal
{

namespace clcode
{

const string base = R"__CODE__(

// -------------------------------------- Types -------------------------------------- 

typedef struct _ScreenDimension
{
    uint width;
    uint height;
} ScreenDimension;

typedef struct _ScreenPosition
{
    uint x;
    uint y;
} ScreenPosition;

typedef struct _SignedScreenPosition
{
    int x;
    int y;
} SignedScreenPosition;

typedef struct _NDCPosition
{
    float x;
    float y;
} NDCPosition;

typedef struct _ColorRGB 
{
    uchar r;
    uchar g;
    uchar b;
} ColorRGB;

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

// ----------------------------------------------------------------------------

// -------------------------------------- Utilities -------------------------------------- 

bool has_stencil(const FrameBuffer buffer)
{
    return buffer.stencil != NULL;
}

bool has_depth(const FrameBuffer buffer)
{
    retrun buffer.depth != NULL;
}

uint index_from_screen(const ScreenPosition pos, const ScreenDimension dim)
{
    return pos.y * dim.width + pos.x;
}

uint from_continuous(const float continuous)
{
    return floor(continous);
}

float from_discrete(const uint discrete)
{
    return discrete + 0.5;
}

uint axis_screen_from_ndc(const float axis_pos, const uint axis_length)
{
    return from_continuous((axis_pos + 1) * 0.5 * (axis_length - 1));
}

void screen_from_ndc(const NDCPosition ndc, const ScreenDimension dim, ScreenPosition* screen)
{
    screen.x = axis_screen_from_ndc(ndc.x, dim.width);
    screen.y = axis_screen_from_ndc(ndc.y, dim.height);
}

int axis_signed_from_ndc(const float axis_pos, const uint axis_length)
{
    return from_continuous((axis_pos + 1) * 0.5 * (axis_length - 1) - axis_length / 2);
}

void signed_from_ndc(const NDCPosition ndc, const ScreenDimension dim, SignedPosition* screen)
{
    screen.x = axis_signed_from_ndc(ndc.x, dim.width);
    screen.y = axis_signed_from_ndc(ndc.y, dim.height);
}

void screen_from_signed(const SignedPosition signed, const ScreenDimension dim, ScreenPosition* screen)
{
    screen.x = signed.x + dim.width / 2;
    screen.y = signed.y + dim.height / 2;
}

// ----------------------------------------------------------------------------

// -------------------------------------- Draw -------------------------------------- 

void apply_fragment_full(const Fragment fragment, FrameBuffer* frame)
{
    uint index = index_from_screen(fragment.position); 

    frame.color[index]   = fragment.color;
    frame.stencil[index] = fragment.primitive;
}

void apply_fragment_no_stencil(const Fragment fragment, FrameBuffer* frame)
{
    uint index = index_from_screen(fragment.position); 

    frame.color[index] = fragment.color;

}

)__CODE__";

}

}

}