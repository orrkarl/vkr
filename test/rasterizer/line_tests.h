#pragma once

#include <math.h>

#include "../includes.h"

#include "test_templates.h"


TEST(RasterizerTest, 2dSingleHorizontalLineRasterTest)
{
    const NRuint dim = 2;
    const NRuint width = 10;
    const NRuint height = 10;
    
    std::vector<NRfloat> lines = {
        -1, 0, 1,     // start
        1, 0, 1       // end
    }; 
    
    const NRuint y = height / 2;
    std::set<std::pair<NRuint, NRuint>> shouldBeOn{};
    for (NRuint i = 0; i < width; ++i)
    {
        shouldBeOn.insert(std::pair<NRuint, NRuint>(i, y));
    }

    testLineRaster(dim, width, height, lines, shouldBeOn);
}

TEST(RasterizerTest, 2dSingleVerticalLine)
{
    const NRuint dim = 2;
    const NRuint width = 10;
    const NRuint height = 10;
    
    std::vector<NRfloat> lines = {
        0, 1, 1,     // start
        0, -1, 1       // end
    }; 
    
    const NRuint x = (width - 1) / 2;
    std::set<std::pair<NRuint, NRuint>> shouldBeOn{};
    
    for (NRuint i = 0; i < height; ++i)
    {
        shouldBeOn.insert(std::pair<NRuint, NRuint>(x, i));
    }

    testLineRaster(dim, width, height, lines, shouldBeOn);
}

TEST(RasterizerTest, 2dShortVerticalLine)
{
    const NRuint dim = 2;
    const NRuint width = 12;
    const NRuint height = 12;
    
    std::vector<NRfloat> lines = {
        0, 0.5, 1,     // start
        0, -1, 1       // end
    }; 
    
    const NRuint x = (width - 1) / 2;
    std::set<std::pair<NRuint, NRuint>> shouldBeOn{};
    
    for (NRuint i = height / 4; i < height; ++i)
    {
        shouldBeOn.insert(std::pair<NRuint, NRuint>(x, i));
    }

    testLineRaster(dim, width, height, lines, shouldBeOn);
}

TEST(RasterizerTest, 2dDiagonalLine)
{
    const NRuint dim = 2;
    const NRuint width = 12;
    const NRuint height = 12;
    
    std::vector<NRfloat> lines = {
        -1, 1, 1,     // start
        1, -1, 1       // end
    }; 
    
    std::set<std::pair<NRuint, NRuint>> shouldBeOn{};
    
    for (NRuint i = 0; i < width && i < height; ++i)
    {
        shouldBeOn.insert(std::pair<NRuint, NRuint>(i, i));
    }

    testLineRaster(dim, width, height, lines, shouldBeOn);
}