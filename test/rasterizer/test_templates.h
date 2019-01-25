#pragma once

#include "../includes.h"

#include <vector>
#include <set>
#include <utility>

#include <pipeline/Stage.h>
#include <pipeline/Rasterizer.h>

void printFrame(const std::vector<NRubyte>& frame, const NRuint width)
{
    for (auto i = 0; i < frame.size(); i += 3)
    {
        if ((i / 3) % width == (width - 1))
        {
            printf("[ %d %d %d ]\n", frame[i], frame[i+1], frame[i+2]);
        }
        else
        {
            printf("[ %d %d %d ] ", frame[i], frame[i+1], frame[i+2]);
        }
    }
}

#ifndef NDEBUG
#define PRINT_FRAME(frame, width) printFrame(frame, width)
#else
#define PRINT_FRAME(frame, width)
#endif // NDEBUG

void comparePixels(const NRubyte* result, const NRubyte* expected, const NRuint w, const NRuint h, const NRuint i)
{
    ASSERT_EQ(result[3 * i], expected[0]) << i % w << ',' << i / w << " R";
    ASSERT_EQ(result[3 * i + 1], expected[1]) << i % w << ',' << i / w << " G";
    ASSERT_EQ(result[3 * i + 2], expected[2]) << i % w << ',' << i / w << " B";
}

void testPointRaster(
    const NRuint dim, 
    const NRuint width, 
    const NRuint height,
    std::vector<NRfloat> vertecies,
    std::set<std::pair<NRuint, NRuint>> shouldBeOn)
{
    nr::Error err = nr::Error::NO_ERROR;
    cl_int error = CL_SUCCESS;
    const auto elemSize = dim + 1;
    const auto vertexCount = vertecies.size() / elemSize;

    nr::__internal::Rasterizer rasterizer(dim, error);
    ASSERT_EQ(error, CL_SUCCESS);

    rasterizer.set(0, 0, width, height);

    cl::Buffer d_src = cl::Buffer(
        CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY,
        vertecies.size() * sizeof(NRfloat), 
        vertecies.data(), 
        &error);
    ASSERT_EQ(error, CL_SUCCESS);

    std::vector<NRubyte> h_dest(width * height * 3, 0);
    cl::Buffer d_dest = cl::Buffer(
        CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, 
        h_dest.size() * sizeof(NRubyte), 
        h_dest.data(), 
        &error);
    ASSERT_EQ(error, CL_SUCCESS);

    auto queue = cl::CommandQueue::getDefault();
    ASSERT_EQ(
        (NRint) rasterizer.rasterize(d_src, d_dest, queue, vertexCount, nr::Primitive::POINTS),
        (NRint) nr::Error::NO_ERROR);
    queue.finish();
    queue.enqueueReadBuffer(d_dest, CL_TRUE, 0, h_dest.size() * sizeof(NRubyte), h_dest.data());

    NRubyte pixel_on[] = { 255, 0, 0 };
    NRubyte pixel_off[] = { 0, 0, 0 };
    
    PRINT_FRAME(h_dest, width);
    
    auto h_destRaw = h_dest.data();

    std::set<NRuint> rawShouldBeOn{};
    for (auto p : shouldBeOn)
    {
        auto res = p.second * width + p.first;
        rawShouldBeOn.insert(res);
    }

    for (auto i = 0; i < h_dest.size() / 3; ++i)
    {
        if (rawShouldBeOn.count(i) > 0) 
            comparePixels(h_destRaw, pixel_on, width, height, i);
        else 
            comparePixels(h_destRaw, pixel_off, width, height, i);
    }
}

void testLineRaster(
    const NRuint dim, 
    const NRuint width, 
    const NRuint height,
    std::vector<NRfloat> vertecies,
    std::set<std::pair<NRuint, NRuint>> shouldBeOn)
{
    nr::Error err = nr::Error::NO_ERROR;
    cl_int error = CL_SUCCESS;
    const auto elemSize = dim + 1;
    const auto lineSize = elemSize * 2;
    const auto vertexCount = vertecies.size() / lineSize;

    nr::__internal::Rasterizer rasterizer(dim, error);
    ASSERT_EQ(error, CL_SUCCESS);

    rasterizer.set(0, 0, width, height);

    cl::Buffer d_src = cl::Buffer(
        CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY,
        vertecies.size() * sizeof(NRfloat), 
        vertecies.data(), 
        &error);
    ASSERT_EQ(error, CL_SUCCESS);

    std::vector<NRubyte> h_dest(width * height * 3, 0);
    cl::Buffer d_dest = cl::Buffer(
        CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, 
        h_dest.size() * sizeof(NRubyte), 
        h_dest.data(), 
        &error);
    ASSERT_EQ(error, CL_SUCCESS);

    auto queue = cl::CommandQueue::getDefault();
    ASSERT_EQ((NRint) rasterizer.rasterize(d_src, d_dest, queue, vertexCount, nr::Primitive::LINES), (NRint) nr::Error::NO_ERROR);
    queue.finish();
    queue.enqueueReadBuffer(d_dest, CL_TRUE, 0, h_dest.size() * sizeof(NRubyte), h_dest.data());

    NRubyte pixel_on[] = { 255, 0, 0 };
    NRubyte pixel_off[] = { 0, 0, 0 };
    
    PRINT_FRAME(h_dest, width);
    
    auto h_destRaw = h_dest.data();

    std::set<NRuint> rawShouldBeOn{};
    for (auto p : shouldBeOn)
    {
        auto res = p.second * width + p.first;
        rawShouldBeOn.insert(res);
    }

    for (auto i = 0; i < h_dest.size() / 3; ++i)
    {
        if (rawShouldBeOn.count(i) > 0) 
            comparePixels(h_destRaw, pixel_on, width, height, i);
        else 
            comparePixels(h_destRaw, pixel_off, width, height, i);
    }
}
