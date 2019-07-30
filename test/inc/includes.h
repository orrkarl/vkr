#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <base/CommandQueue.h>
#include <base/Module.h>

#include <general/predefs.h>

#include <rendering/Render.h>

#include <utils/converters.h>


#include <array>

#pragma once

#define ASSERT_SUCCESS(expr) ASSERT_PRED1(isSuccess, (expr))

const float TOLERANCE = 0.00001f;
const double DOUBLE_TOLERANCE = 0.000000001;

extern nr::Context defaultContext;
extern nr::Device defaultDevice;
extern nr::CommandQueue defaultCommandQueue;

template <typename T>
testing::AssertionResult validateBuffer(const nr::string& name, const nr_uint countX, const nr_uint countY, const T* expected, const T* actual, const nr_uint maxDiffs = 5)
{
	std::vector<std::pair<nr_uint, nr_uint>> diffIndices;

	bool diffOverflow = false;

	for (auto i = 0u; i < dimensions.height; ++i)
	{
		for (auto j = 0u; j < dimensions.width; ++j)
		{
			auto idx = i * dimensions.width + j;
			if (expected[idx] != actual[idx])
			{
				if (diffIndices.size() < maxDiffs)
				{
					diffIndices.push_back({ i, j });
				}
				else
				{
					diffOverflow = true;
				}
			}
		}
	}

	if (diffIndices.empty()) return testing::AssertionSuccess();

	auto ret = testing::AssertionFailure();

	ret << "Unexpected " << name << " buffer elements:\n";
	for (auto i = 0u; i < maxDiffs; ++i)
	{
		nr_uint x = diffIndices[i].first;
		nr_uint y = diffIndices[i].second;
		nr_uint idx = y * countX + x;
		ret << "\tAt (" << x << ", " << y << "): " << "expected - " << expected[idx] << ", actual - " << actual[idx] << "\n";
	}
	if (diffOverflow)
	{
		ret << "\t...\n";
	}

	return ret;
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

typedef nr_uint Index;
typedef nr_float Depth;

struct Fragment
{
    ScreenPosition position;
    nr::RawColorRGBA color;
    Depth depth;
};

// convert 2d index from 2d array to 1d flat array index
nr_uint index_from_screen(const ScreenPosition& position, const nr::ScreenDimension& dim);

// Test a module's compilation status
void testCompilation(const nr::Module::Options options, nr::string configurationName, std::initializer_list<nr::string> codes);

// Convert Screen Coordinates to NDC
NDCPosition ndcFromScreen(const ScreenPosition screen, const nr::ScreenDimension& screenDim);

// Check if a status value indicates success, return well formatted message O.W
testing::AssertionResult isSuccess(const cl_status& err);

nr::Module::Options mkStandardOptions(const nr_uint dim);

cl_status init();

void destroy();
