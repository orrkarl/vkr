#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <general/predefs.h>

#pragma once

const float TOLERANCE = 0.00001f;
const double DOUBLE_TOLERANCE = 0.000000001;

template<NRuint dim>
struct Point
{
    NRfloat values[dim];
};

template<NRuint dim>
struct Simplex
{
    Point<dim> points[dim];
};

template<NRuint dim>
struct Triangle
{
    Point<dim> points[3];
};

struct ScreenPosition
{
    NRuint x;
    NRuint y;

    bool operator==(const ScreenPosition& other) const
    {
        return x == other.x && y == other.y;
    }

    friend std::ostream& operator<<(std::ostream& os, const ScreenPosition& pos)
    {
        return os << "ScreenPosition{" << pos.x << ", " << pos.y << "}";
    }
};

struct NDCPosition
{
    NRfloat x;
    NRfloat y;
};

struct RawColorRGB
{
    NRubyte r, g, b;
};

typedef NRuint Index;
typedef NRfloat Depth;

struct Fragment
{
    ScreenPosition position;
    RawColorRGB color;
    Index stencil;
    Depth depth;
};

void testCompilation(const char* options, nr::string configurationName, std::initializer_list<nr::string> codes);


