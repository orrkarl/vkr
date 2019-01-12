#include "includes.h"

#include <pipeline/Stage.h>
#include <pipeline/Rasterizer.h>

#include <chrono>
#include <ctime>  


void comparePixels(const NRubyte* result, const NRubyte* expected, const NRuint i)
{
    ASSERT_EQ(result[3 * i], expected[0]) << i % 10 << ',' << i / 10 << " R";
    ASSERT_EQ(result[3 * i + 1], expected[1]) << i % 10 << ',' << i / 10 << " G";
    ASSERT_EQ(result[3 * i + 2], expected[2]) << i % 10 << ',' << i / 10 << " B";
}

TEST(RasterizerTest, LoaderSanityTest)
{
    nr::Error err = nr::Error::NO_ERROR;
    nr::Rasterizer rasterizer(3, err);
    ASSERT_TRUE(nr::error::isSuccess(err));
}

TEST(RasterizerTest, 2dViewPortTest)
{
    nr::Error err = nr::Error::NO_ERROR;
    cl_int error = CL_SUCCESS;
    nr::Rasterizer rasterizer(2, err);

    const NRuint width = 10;
    const NRuint height = 10;
    const NRuint vertexCount = 50;

    rasterizer.set(0, 0, width, height);

    ASSERT_TRUE(nr::error::isSuccess(err));

    std::vector<NRfloat> h_src(vertexCount * 2, 0.0f);
    
    // Bottom right corner
    h_src[0] = 1;
    h_src[1] = 1;

    // Top left corner
    h_src[2] = -1;
    h_src[3] = -1;

    // Top right corner
    h_src[4] = 1;
    h_src[5] = -1;

    // Bottom left corner
    h_src[6] = -1;
    h_src[7] = 1;

    cl::Buffer d_src = cl::Buffer(
        CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY,
        h_src.size() * sizeof(NRfloat), 
        h_src.data(), 
        &error);
    ASSERT_TRUE(nr::error::isSuccess(err = nr::utils::fromCLError(error)));

    std::vector<NRubyte> h_dest(width * height * 3, 0);
    cl::Buffer d_dest = cl::Buffer(
        CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, 
        h_dest.size() * sizeof(NRubyte), 
        h_dest.data(), 
        &error);
    ASSERT_TRUE(nr::error::isSuccess(err = nr::utils::fromCLError(error)));

    auto queue = cl::CommandQueue::getDefault();
    ASSERT_EQ((NRint) rasterizer.update(queue), (NRint) nr::Error::NO_ERROR);
    ASSERT_EQ((NRint) rasterizer.apply(d_src, d_dest, queue, vertexCount), (NRint) nr::Error::NO_ERROR);
    queue.finish();
    queue.enqueueReadBuffer(d_dest, CL_TRUE, 0, h_dest.size() * sizeof(NRubyte), h_dest.data());

    NRubyte pixel_on[] = { 255, 0, 0 };
    NRubyte pixel_off[] = { 0, 0, 0 };
    
    auto h_destRaw = h_dest.data();

    comparePixels(h_destRaw, pixel_on, 0);                                  // Top left corner
    comparePixels(h_destRaw, pixel_on, width - 1);                          // Top right corner
    comparePixels(h_destRaw, pixel_on, (height - 1) * height);              // Bottom left corner
    comparePixels(h_destRaw, pixel_on, (height - 1) * height + width - 1);  // Bottom right corner

    comparePixels(h_destRaw, pixel_off, 5);
    comparePixels(h_destRaw, pixel_off, 70);
}