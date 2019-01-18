#pragma once

#include "../includes.h"

#include "test_templates.h"

TEST(RasterizerTest, LoaderSanityTest)
{
    cl_int err;
    nr::__internal::Rasterizer rasterizer(3, err);
    ASSERT_EQ(err, CL_SUCCESS) << rasterizer.getCompilationLog();
}

TEST(RasterizerTest, 2dPointRasterTest)
{
    const NRuint dim = 2;
    const NRuint width = 10;
    const NRuint height = 10;
    testPointRaster(dim, width, height);
}

TEST(RasterizerTest, 3dPointRasterTest)
{
    const NRuint dim = 3;
    const NRuint width = 10;
    const NRuint height = 10;
    testPointRaster(dim, width, height);
}

TEST(RasterizerTest, 11dPointRasterTest)
{
    const NRuint dim = 11;
    const NRuint width = 10;
    const NRuint height = 10;
    testPointRaster(dim, width, height);
}
