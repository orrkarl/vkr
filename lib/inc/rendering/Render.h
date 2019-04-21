#pragma once

#include "../general/predefs.h"
#include "../base/Buffer.h"

namespace nr
{

struct NR_SHARED_EXPORT ScreenDimension
{
    NRuint width;
    NRuint height;
};

struct NR_SHARED_EXPORT FrameBuffer
{
    Buffer color;
    Buffer stencil;
    Buffer depth;
};

}
