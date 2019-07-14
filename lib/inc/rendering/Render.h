#pragma once

#include "../general/predefs.h"
#include "../base/Buffer.h"

#include <iostream>

namespace nr
{

// To make sure some of the more importent structs won't be allined by the compiler (e.g. RawColorRGB), as most of them are ment to be copied to/from OpenCL 
#pragma pack(push, 1)

struct ScreenDimension
{
    nr_uint width, height;

    friend std::ostream& operator<<(std::ostream& os, const ScreenDimension& dim)
    {
        return os << "ScreenDimension{" << dim.width << ", " << dim.height << "}";
    }
};

struct RawColorRGBA
{
    nr_ubyte r, g, b, a;

    bool operator==(const RawColorRGBA& other) const
    {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

    friend std::ostream& operator<<(std::ostream& os, const RawColorRGBA& color)
    {
        return os << "(" << (nr_uint) color.r << "," << (nr_uint) color.g << "," << (nr_uint) color.b << "," << (nr_uint) color.a << ')';
    }
};

struct FrameBuffer
{
    Buffer<RawColorRGBA> color;
    Buffer<nr_float> depth;
};

template<nr_uint dim>
struct Point
{
    nr_float values[dim + 1];

    bool operator==(const Point& other) const
    {
        for (auto i = 0; i < dim + 1; ++i) 
            if (std::abs(values[i] - other[i]) > 10e-6)
                return false;

        return true;
    }

    friend std::ostream& operator<<(std::ostream& os, const Point<dim>& self)
    {
        os << "Point<" << dim << ">{ ";
        for (auto i = 0; i < dim; ++i)
        {
            os << self[i] << ", ";
        }
        return os << self[dim] << " }";
    }

    nr_float operator[](const nr_uint index) const { return values[index]; }

	nr_float& operator[](const nr_uint index) { return values[index]; }
};

// represents a dim-1 dimensional simplex, embedded in 'dim' dimensional space
template<nr_uint dim>
struct Simplex
{
    Point<dim> points[dim];

	Point<dim> operator[](const nr_uint index) const { return points[index]; }

	Point<dim>& operator[](const nr_uint index) { return points[index]; }
};

template<nr_uint dim>
struct Triangle
{
    Point<dim> points[3];

	bool operator==(const Triangle<dim>& other) const
	{
		for (auto i = 0; i < 3; ++i)
			if (points[i] != other[i])
				return false;

		return true;
	}

	friend std::ostream& operator<<(std::ostream& os, const Triangle& tri)
	{
		return os << "Triangle{" << tri.points[0] << ", " << tri.points[1] << ", " << tri.points[2] << "}";
	}

	Point<dim> operator[](const nr_uint index) const { return points[index]; }

	Point<dim>& operator[](const nr_uint index) { return points[index]; }
};

#pragma pack(pop)

}