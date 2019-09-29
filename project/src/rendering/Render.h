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
 */
struct Vertex
{
    nr_float values[4];

	Vertex()
		: values{ 0 }
	{
	}

	Vertex(const nr_float x, const nr_float y, const nr_float z)
		: values{x, y, z, 1}
	{
	}

	Vertex(const nr_float x, const nr_float y, const nr_float z, const nr_float w)
		: values{ x, y, z, w }
	{
	}

	bool operator==(const Vertex& other) const
	{
		for (auto i = 0; i < 4; ++i)
		{
			if (std::abs(values[i] - other[i]) > 10e-6)
			{
				return false;
			}
		}

        return true;
    }

	bool operator!=(const Vertex& other) const
	{
		return !(*this == other);
	}

    friend std::ostream& operator<<(std::ostream& os, const Vertex& self)
    {
        os << "Point{ ";
        for (auto i = 0; i < 3; ++i)
        {
            os << self[i] << ", ";
        }
        return os << self[3] << " }";
    }

    nr_float operator[](const nr_uint index) const { return values[index]; }

	nr_float& operator[](const nr_uint index) { return values[index]; }
};

/**
 * @brief Triangle, embedded in 3 dimensional space (with homogenous coordinates)
 */
struct Triangle
{
    Vertex points[3];

	bool operator==(const Triangle& other) const
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

	Vertex operator[](const nr_uint index) const { return points[index]; }

	Vertex& operator[](const nr_uint index) { return points[index]; }
};

/**
 * @brief Rasterizer primitive type
 * 
 */
enum class Primitive : nr_uint
{
	TRIANGLE
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