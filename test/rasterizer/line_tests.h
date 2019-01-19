#pragma once

#include <math.h>

#include "../includes.h"

#include "test_templates.h"


TEST(RasterizerTest, 2dVerticalLineRasterTest)
{
    const NRuint dim = 2;
    const NRuint width = 10;
    const NRuint height = 10;
    
    std::vector<NRfloat> lines = {
        -1, 0, 1,     // start
        1, 0, 1       // end
    }; 
    
    const NRuint y = (height - 1) / 2;
    std::set<std::pair<NRuint, NRuint>> shouldBeOn{};
    for (NRuint i = 0; i < width; ++i)
    {
        shouldBeOn.insert(std::pair<NRuint, NRuint>(i, y));
    }

    testLineRaster(dim, width, height, lines, shouldBeOn);
}