#pragma once

#include "../includes.h"

#include "test_templates.h"

TEST(RasterizerTest, LoaderSanityTest)
{
    cl_int err;
    nr::__internal::Rasterizer rasterizer(3, err);
    ASSERT_EQ(err, CL_SUCCESS) << rasterizer.getCompilationLog();
}

TEST(RasterizerTest, PointSanityTest)
{
    const NRuint dim = 3;
    const NRuint width = 10;
    const NRuint height = 10;

    const std::vector<NRfloat> vertecies = {
        -1, 1, 0, 1,
        -1, -1, 0, 1,
        1, -1, 0, 1,
        1, 1, 0, 1
    };

    const std::set<std::pair<NRuint, NRuint>> shouldBeOn = {
        std::pair<NRuint, NRuint>(0, 0),
        std::pair<NRuint, NRuint>(0, width - 1),
        std::pair<NRuint, NRuint>(height - 1, 0),
        std::pair<NRuint, NRuint>(height - 1, width - 1)
    };

    testPointRaster(dim, width, height, vertecies, shouldBeOn);
}

TEST(RasterizerTest, SpecificPointTest)
{
    const NRuint dim = 3, width = 10, height = 10;
    
    const std::vector<NRfloat> vertecies = {
        -0.5, 0.5, 200, 1
    };

    const std::set<std::pair<NRuint, NRuint>> shouldBeOn = {
        std::pair<NRuint, NRuint>(width / 4, height / 2 - height / 4)
    };

    testPointRaster(dim, width, height, vertecies, shouldBeOn);
}