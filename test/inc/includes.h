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

    bool operator==(const Point& other) const
    {
        for (auto i = 0; i < dim; ++i) 
            if (values[i] != other[i])
                return false;

        return true;
    }

    friend std::ostream& operator<<(std::ostream& os, const Point<dim>& self)
    {
        os << "Point<" << dim << ">{ ";
        for (auto i = 0; i < dim - 1; ++i)
        {
            os << self[i] << ", ";
        }
        return os << self[dim - 1] << " }";
    }

    NRfloat operator[](const NRuint index) const { return values[index]; }
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

    bool operator==(const NDCPosition& other) const
    {
        return x == other.x && y == other.y;
    }

    friend std::ostream& operator<<(std::ostream& os, const NDCPosition& pos)
    {
        return os << "NDCPosition{" << pos.x << ", " << pos.y << "}";
    }
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
    Depth depth;
};

// convert 2d index from 2d array to 1d flat array index
NRuint index_from_screen(const ScreenPosition& position, const nr::ScreenDimension& dim);

// Test a module's compilation status
void testCompilation(const char* options, nr::string configurationName, std::initializer_list<nr::string> codes);

// Convert Screen Coordinates to NDC
NDCPosition ndcFromScreen(const ScreenPosition screen, const nr::ScreenDimension& screenDim);

// Check if a cl_int value indicates success, return well formatted message O.W
testing::AssertionResult isSuccess(const cl_int& err);

