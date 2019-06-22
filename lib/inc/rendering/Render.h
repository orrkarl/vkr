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

struct FrameBuffer
{
    Buffer<RawColorRGB> color;
    Buffer<NRfloat> depth;
};

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

    friend std::ostream& operator<<(std::ostream& os, const Triangle& tri)
    {
        return os << "Triangle{" << tri.points[0] << ", " << tri.points[1] << ", " << tri.points[2] << "}";
    }
};

}