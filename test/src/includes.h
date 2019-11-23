#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <base/CommandQueue.h>
#include <base/Module.h>

#include <predefs.h>

#include <rendering/Render.h>

#include <utils/converters.h>

#include <array>
#include <cmath>
#include <fstream>
#include <functional>
#include <limits>

#pragma once

#define ASSERT_SUCCESS(expr) ASSERT_TRUE(isSuccess(expr))

const float TOLERANCE = 0.00001f;
const double DOUBLE_TOLERANCE = 0.000000001;

extern nr::Context defaultContext;
extern nr::Device defaultDevice;
extern nr::CommandQueue defaultCommandQueue;

template <typename T, nr_uint Width, nr_uint Height, typename Comparator>
testing::AssertionResult validateBuffer(const nr::string& name, const T expected[Height][Width], const T actual[Height][Width], Comparator cmp, const nr_uint maxDiffs = 10)
{
	std::vector<std::pair<nr_uint, nr_uint>> diffIndices;

	bool diffOverflow = false;
	nr_ulong totalDiffs = 0;

	for (auto y = 0u; y < Height; ++y)
	{
		for (auto x = 0u; x < Width; ++x)
		{
			if (!cmp(expected[y][x], actual[y][x]))
			{
				if (diffIndices.size() < maxDiffs)
				{
					diffIndices.push_back({ x, y });
				}
				else
				{
					diffOverflow = true;
				}

				totalDiffs += 1;
			}
		}
	}

	if (diffIndices.empty()) return testing::AssertionSuccess();

	auto ret = testing::AssertionFailure();

	ret << "Unexpected " << name << " buffer elements:";
	for (auto i = 0u; i < diffIndices.size(); ++i)
	{
		nr_uint x = diffIndices[i].first;
		nr_uint y = diffIndices[i].second;
		ret << "\n\tAt (" << x << ", " << y << "): " << "expected - " << expected[y][x] << ", actual - " << actual[y][x];
	}
	if (diffOverflow)
	{
		ret << "\n\t... [ " << totalDiffs << " total differences ] ";
	}

	//std::ofstream expectedLog;
	//expectedLog.open(name + "-expected.log");
	//
	//std::ofstream actualLog;
	//actualLog.open(name + "-actual.log");
	//
	//for (nr_int y = Height - 1; y >= 0; --y)
	//{
	//	for (auto x = 0u; x < Width; ++x)
	//	{
	//		expectedLog << expected[y][x]<< " ";
	//		actualLog << actual[y][x] << " ";
	//	}
	//	expectedLog << '\n';
	//	actualLog << '\n';
	//}
	//
	//expectedLog.close();
	//actualLog.close();

	return ret;
}

template <typename T, nr_uint Width, nr_uint Height>
testing::AssertionResult validateBuffer(const nr::string& name, const T expected[Height][Width], const T actual[Height][Width])
{
	return validateBuffer<T, Width, Height>(name, expected, actual, std::equal_to<T>());
}

template <nr_uint Width, nr_uint Height>
testing::AssertionResult validateBuffer(const nr::string& name, const nr_float expected[Height][Width], const nr_float actual[Height][Width])
{
	return validateBuffer<nr_float, Width, Height>(name, expected, actual, [](const nr_float f0, const nr_float f1) { return std::abs(f0 - f1) < 10e-5; });
}

template <typename T, nr_uint Width, nr_uint Height>
void printBuffer(const T buffer[Height][Width])
{
	for (nr_int y = Height - 1; y >= 0; --y)
	{
		for (auto x = 0u; x < Width; ++x)
		{
			std::cout << buffer[y][x] << " ";
		}
		std::cout << "\n";
	}
}


struct ScreenPosition
{
    nr_uint x;
    nr_uint y;

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
    nr_float x;
    nr_float y;

    bool operator==(const NDCPosition& other) const
    {
        return x == other.x && y == other.y;
    }

    friend std::ostream& operator<<(std::ostream& os, const NDCPosition& pos)
    {
        return os << "NDCPosition{" << pos.x << ", " << pos.y << "}";
    }
};

struct Bin
{
	nr_uint width;
	nr_uint height;
	nr_uint x;
	nr_uint y;
};

using Index = nr_uint;

struct Fragment
{
    ScreenPosition position;
    nr::RawColorRGBA color;
    nr::Depth depth;
};

// convert 2d index from 2d array to 1d flat array index
nr_uint indexFromScreen(const ScreenPosition& position, const nr::ScreenDimension& dim);

// Test a module's compilation status
void testCompilation(const nr::Module::Options options, nr::string configurationName, std::initializer_list<nr::string> codes);

// Convert Screen Coordinates to NDC
NDCPosition ndcFromScreen(const ScreenPosition screen, const nr::ScreenDimension& screenDim);

NDCPosition ndcFromPixelMid(const ScreenPosition& screen, const nr::ScreenDimension& dim);

// Check if a status value indicates success, return well formatted message O.W
testing::AssertionResult isSuccess(const cl_status& err);

nr::Module::Options mkStandardOptions();

cl_status init();

void destroy();

// Because MSVC doesn't evaluate constexpr as well as GCC...
constexpr nr_uint compileTimeCeil(const nr_float f)
{
	const nr_uint as_uint = static_cast<nr_uint>(f);
	return as_uint == f ? as_uint : as_uint + 1;
}

