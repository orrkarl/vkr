#pragma once

#include "../includes.h"

#include "test_templates.h"


TEST(RasterizerTest, 2dVerticalLineRasterTest)
{
    const NRuint dim = 2;
    const NRuint width = 10;
    const NRuint height = 10;
    
    std::vector<NRfloat> lines = {
        -0.5, 0, 1,
        0.5, 0, 1
    };
    
    std::set<std::pair<NRuint, NRuint>> shouldBeOn{};
    for (NRuint i = width / 4; i <= width * 3 / 4; ++i)
    {
        shouldBeOn.insert(std::pair<NRuint, NRuint>(i, height / 2));
    }

    testLineRaster(dim, width, height, lines, shouldBeOn);
}