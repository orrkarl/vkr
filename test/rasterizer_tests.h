#include "includes.h"

#include <pipeline/Stage.h>
#include <pipeline/Rasterizer.h>

void comparePixels(const NRubyte* result, const NRubyte* expected, const NRuint i)
{
    ASSERT_EQ(result[3 * i], expected[0]) << i % 10 << ',' << i / 10 << " R";
    ASSERT_EQ(result[3 * i + 1], expected[1]) << i % 10 << ',' << i / 10 << " G";
    ASSERT_EQ(result[3 * i + 2], expected[2]) << i % 10 << ',' << i / 10 << " B";
}

void testPointRaster(const NRuint dim, const NRuint width, const NRuint height)
{
    const NRuint vertexCount = 4;
    nr::Error err = nr::Error::NO_ERROR;
    cl_int error = CL_SUCCESS;

    nr::__internal::Rasterizer rasterizer(dim, error);
    ASSERT_EQ(error, CL_SUCCESS);

    rasterizer.set(0, 0, width, height);

    std::vector<NRfloat> h_src(vertexCount * dim, 0.0f);
    
    // Bottom right corner
    h_src[0] = 1;
    h_src[1] = 1;

    // Top left corner
    h_src[dim] = -1;
    h_src[dim + 1] = -1;

    // Top right corner
    h_src[2 * dim] = 1;
    h_src[2 * dim + 1] = -1;

    // Bottom left corner
    h_src[3 * dim] = -1;
    h_src[3 * dim + 1] = 1;

    cl::Buffer d_src = cl::Buffer(
        CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY,
        h_src.size() * sizeof(NRfloat), 
        h_src.data(), 
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
    ASSERT_EQ((NRint) rasterizer.rasterize(d_src, d_dest, queue, vertexCount, nr::Primitive::POINTS), (NRint) nr::Error::NO_ERROR);
    queue.finish();
    queue.enqueueReadBuffer(d_dest, CL_TRUE, 0, h_dest.size() * sizeof(NRubyte), h_dest.data());

    NRubyte pixel_on[] = { 255, 0, 0 };
    NRubyte pixel_off[] = { 0, 0, 0 };
    
    auto h_destRaw = h_dest.data();

    const NRuint TOP_LEFT = 0;
    const NRuint TOP_RIGHT = width - 1;
    const NRuint BOTTOM_LEFT = (height - 1) * height;
    const NRuint BOTTOM_RIGHT = (height - 1) * height + width - 1;
    
    const std::set<NRuint> shouldBeOn{ TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT };
    const std::set<NRuint> shouldBeOff{ (NRuint) 0.7 * height * width, width / 2 };

    for (const auto& i : shouldBeOn)
    {
        comparePixels(h_destRaw, pixel_on, i);
    }

    for (const auto& i : shouldBeOff)
    {
        if (shouldBeOn.count(i) == 0)
        {
            comparePixels(h_destRaw, pixel_off, i);
        }
    }
}

void testLineRaster(const NRuint dim, const NRuint width, const NRuint height)
{
    const NRuint vertexCount = 4;
    const NRuint elementCount = vertexCount * 2;
    nr::Error err = nr::Error::NO_ERROR;
    cl_int error = CL_SUCCESS;

    nr::__internal::Rasterizer rasterizer(dim, error);
    ASSERT_EQ(error, CL_SUCCESS);

    rasterizer.set(0, 0, width, height);

    std::vector<NRfloat> h_src(elementCount * dim, 0.0f);

    // First line :: Start
    h_src[0] = 1;
    h_src[1] = 1;

    // First line :: End
    h_src[dim] = -1;
    h_src[dim + 1] = -1;

    // Second line :: Start
    h_src[2 * dim] = 1;
    h_src[2 * dim + 1] = -1;

    // Second line :: End
    h_src[3 * dim] = -1;
    h_src[3 * dim + 1] = 1;

    cl::Buffer d_src = cl::Buffer(
        CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY,
        h_src.size() * sizeof(NRfloat), 
        h_src.data(), 
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
    
    auto h_destRaw = h_dest.data();

    const NRuint TOP_LEFT = 0;
    const NRuint TOP_RIGHT = width - 1;
    const NRuint BOTTOM_LEFT = (height - 1) * height;
    const NRuint BOTTOM_RIGHT = (height - 1) * height + width - 1;
    
    const std::set<NRuint> shouldBeOn{ TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT };
    const std::set<NRuint> shouldBeOff{ (NRuint) 0.7 * height * width, width / 2 };

    for (const auto& i : shouldBeOn)
    {
        comparePixels(h_destRaw, pixel_on, i);
    }

    for (const auto& i : shouldBeOff)
    {
        if (shouldBeOn.count(i) == 0)
        {
            comparePixels(h_destRaw, pixel_off, i);
        }
    }
}

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

TEST(RasterizerTest, 2dLineRasterTest)
{
    const NRuint dim = 2;
    const NRuint width = 10;
    const NRuint height = 10;
    testLineRaster(dim, width, height);
}