#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <general/predefs.h>
#include <utils/converters.h>
#include <rendering/Render.h>
#include <base/Module.h>

#pragma once

const float TOLERANCE = 0.00001f;
const double DOUBLE_TOLERANCE = 0.000000001;

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
void testCompilation(const nr::__internal::Module::Options options, nr::string configurationName, std::initializer_list<nr::string> codes);

// Convert Screen Coordinates to NDC
NDCPosition ndcFromScreen(const ScreenPosition screen, const nr::ScreenDimension& screenDim);

// Check if a cl_int value indicates success, return well formatted message O.W
testing::AssertionResult isSuccess(const cl_int& err);

nr::__internal::Module::Options mkStandardOptions(const NRuint dim);
