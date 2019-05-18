#pragma once

#include "../general/predefs.h"
#include "../base/Buffer.h"

namespace nr
{

struct ScreenDimension
{
    NRuint width, height;

    friend std::ostream& operator<<(std::ostream& os, const ScreenDimension& dim)
    {
        return os << "ScreenDimension{" << dim.width << ", " << dim.height << "}";
    }
};

struct FrameBuffer
{
    Buffer color, depth;
};

}