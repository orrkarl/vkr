#pragma once

#include "../general/predefs.h"
#include "../base/Buffer.h"

#include <iostream>

namespace nr
{

struct ScreenDimension
{
    nr_uint width, height;

    friend std::ostream& operator<<(std::ostream& os, const ScreenDimension& dim)
    {
        return os << "ScreenDimension{" << dim.width << ", " << dim.height << "}";
    }
};

struct RawColorRGB
{
    nr_ubyte r, g, b;

    bool operator==(const RawColorRGB& other) const
    {
        return r == other.r && g == other.g && b == other.b;
    }

    friend std::ostream& operator<<(std::ostream& os, const RawColorRGB& color)
    {
        return os << "(" << (nr_uint) color.r << "," << (nr_uint) color.g << "," << (nr_uint) color.b << ')';
    }
};

struct FrameBuffer
{
    Buffer<RawColorRGB> color;
    Buffer<nr_float> depth;
};

template<nr_uint dim>
struct Point
{
    nr_float values[dim];

    bool operator==(const Point& other) const
    {
        for (auto i = 0; i < dim; ++i) 
            if (std::abs(values[i] - other[i]) > 10e-6)
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

    nr_float operator[](const nr_uint index) const { return values[index]; }
};

template<nr_uint dim>
struct Simplex
{
    Point<dim> points[dim];
};

template<nr_uint dim>
struct Triangle
{
    Point<dim> points[3];

    friend std::ostream& operator<<(std::ostream& os, const Triangle& tri)
    {
        return os << "Triangle{" << tri.points[0] << ", " << tri.points[1] << ", " << tri.points[2] << "}";
    }
};

}