#pragma once

#include <inc/includes.h>

#define R (0)
#define G (1)
#define B (2)
#define DEPTH (3)

struct Quad
{
    NRfloat top_left[4], top_right[4], bottom_left[4], bottom_right[4];
    NRuint primitive;
    NRuint position[2];
};

struct ColorRGB
{
    NRfloat r, g, b;

    bool operator==(const ColorRGB& other)
    {
        return r == other.r && g == other.g && b == other.b;
    }
};

NRbool isQuadValid(const Quad& quad)
{
    return quad.bottom_left[DEPTH] == NAN;
}

