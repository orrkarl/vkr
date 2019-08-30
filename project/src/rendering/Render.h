/**
 * @file
 * @author Orr Karl (karlor041@gmail.com)
 * @brief rendering related data structures
 * @version 0.6.0
 * @date 2019-08-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../predefs.h"

#include "../base/Buffer.h"

#include <iomanip>
#include <iostream>

namespace nr
{

// To make sure some of the more importent structs won't be allined by the compiler (e.g. RawColorRGB), as most of them are ment to be copied to/from OpenCL 
#pragma pack(push, 1)

/**
 * @brief Dimensions (width and height) of the rendering target
 * 
 */
struct ScreenDimension
{
    nr_uint width, height;

    /**
     * @brief get the pixel count in this screen dimension
     * 
     * @return  pixel count
     */
	nr_uint getTotalSize() const { return width * height; }

    friend std::ostream& operator<<(std::ostream& os, const ScreenDimension& dim)
    {
        return os << "ScreenDimension{" << dim.width << ", " << dim.height << "}";
    }
};

/**
 * @brief Color buffer contents
 * 
 * As of right now, the color buffer only supports raw RGBA
 */
struct RawColorRGBA
{
    nr_ubyte r, g, b, a;

    bool operator==(const RawColorRGBA& other) const
    {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

	bool operator!=(const RawColorRGBA& other) const
	{
		return !(operator==(other));
	}

    friend std::ostream& operator<<(std::ostream& os, const RawColorRGBA& color)
    {
        return os << "(" << std::setw(3) << (nr_uint) color.r << "," << std::setw(3) << (nr_uint) color.g << "," << std::setw(3) << (nr_uint) color.b << "," << std::setw(3) << (nr_uint) color.a << ')';
    }
};

using Depth = nr_float;

struct FrameBuffer
{
    Buffer color;
    Buffer depth;
};

/**
 * @brief A homogenous point in N-d space
 * 
 * @tparam dim point dimension
 */
template<nr_uint dim>
struct Vertex
{
    nr_float values[dim + 1];

    bool operator==(const Vertex& other) const
    {
        for (auto i = 0; i < dim + 1; ++i) 
            if (std::abs(values[i] - other[i]) > 10e-6)
                return false;

        return true;
    }

    friend std::ostream& operator<<(std::ostream& os, const Vertex<dim>& self)
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

/**
 * @brief A N-1 dimensional simplex, embedded in N dimensional space (with homogenous coordinates)
 * 
 * @tparam dim point dimension
 */
template<nr_uint dim>
struct Simplex
{
    Vertex<dim> points[dim];

	Vertex<dim> operator[](const nr_uint index) const { return points[index]; }

	Vertex<dim>& operator[](const nr_uint index) { return points[index]; }
};

/**
 * @brief Triangle, embedded in N dimensional space (with homogenous coordinates)
 * 
 * @tparam dim 
 */
template<nr_uint dim>
struct Triangle
{
    Vertex<dim> points[3];

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

	Vertex<dim> operator[](const nr_uint index) const { return points[index]; }

	Vertex<dim>& operator[](const nr_uint index) { return points[index]; }
};

/**
 * @brief Rasterizer primitive type
 * 
 */
enum class Primitive : nr_uint
{
	SIMPLEX
};

namespace detail
{

/**
 * @brief Bin rasterizer queue configuration
 * 
 * Refer to BinRasterizer for detailed explenation
 */
struct BinQueueConfig
{
	nr_uint binWidth;
	nr_uint binHeight;
	nr_uint queueSize;
};

}

#pragma pack(pop)

}