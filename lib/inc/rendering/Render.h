#pragma once

#include "../general/predefs.h"
#include "../base/Buffer.h"

namespace nr
{

struct ScreenDimension
{
    NRuint width, height;
};

struct FrameBuffer
{
    Buffer color, depth;
};

}