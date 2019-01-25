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
        -1, 0, 1,     
        1, 0, 1       
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
        0, 1, 1,     
        0, -1, 1     
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
        0, 0.5, 1,   
        0, -1, 1     
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
        -1, 1, 1,
        1, -1, 1
    }; 
    
    std::set<std::pair<NRuint, NRuint>> shouldBeOn{};
    
    for (NRuint i = 0; i < width && i < height; ++i)
    {
        shouldBeOn.insert(std::pair<NRuint, NRuint>(i, i));
    }

    testLineRaster(dim, width, height, lines, shouldBeOn);
}

TEST(RasterizerTest, 2dManyLines)
{
    const NRuint dim = 2;
    const NRuint width = 10;
    const NRuint height = 10;
    
    std::vector<NRfloat> lines = {
        -1, 0.8, 1,   // Line 1
        0, 0.8, 1,

        -1, -1, 1,    // Line 2
        1, 1, 1,

        -0.7, -1, 1,  // Line 3  
        -0.7, 1, 1,

        0.2, -1, 1,   // Line 4
        1, -0.7, 0
    }; 
    
    std::set<std::pair<NRuint, NRuint>> shouldBeOn{};
    
    // Line 1
    shouldBeOn.insert(std::pair<NRuint, NRuint>(0, 1));
    shouldBeOn.insert(std::pair<NRuint, NRuint>(1, 1));
    shouldBeOn.insert(std::pair<NRuint, NRuint>(2, 1));
    shouldBeOn.insert(std::pair<NRuint, NRuint>(3, 1));
    shouldBeOn.insert(std::pair<NRuint, NRuint>(4, 1));

    // Line 2
    for (auto i = 0; i < width && i < height; ++i)
        shouldBeOn.insert(std::pair<NRuint, NRuint>(i, 9 - i));

    // Line 3
    for (auto i = 0; i < height; ++i)
        shouldBeOn.insert(std::pair<NRuint, NRuint>(1, i));

    // Line 4
    shouldBeOn.insert(std::pair<NRuint, NRuint>(5, 9));
    shouldBeOn.insert(std::pair<NRuint, NRuint>(6, 9));
    shouldBeOn.insert(std::pair<NRuint, NRuint>(7, 9));
    shouldBeOn.insert(std::pair<NRuint, NRuint>(8, 8));
    shouldBeOn.insert(std::pair<NRuint, NRuint>(9, 8));

    testLineRaster(dim, width, height, lines, shouldBeOn);
}