#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <general/predefs.h>
#include <utils/converters.h>
#include <rendering/Render.h>

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

    bool operator==(const RawColorRGB& other) const
    {
        return r == other.r && g == other.g && b == other.b;
    }

    friend std::ostream& operator<<(std::ostream& os, const RawColorRGB& color)
    {
        return os << "(" << (NRuint) color.r << "," << (NRuint) color.g << "," << (NRuint) color.b << ')';
    }
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

NRuint index_from_screen(const ScreenPosition& position, const nr::ScreenDimension& dim);

void testCompilation(const char* options, nr::string configurationName, std::initializer_list<nr::string> codes);

NDCPosition ndcFromScreen(const ScreenPosition screen, const nr::ScreenDimension& screenDim);

testing::AssertionResult isSuccess(const cl_int& err);

testing::AssertionResult isSuccess(const nr::Error& err);


