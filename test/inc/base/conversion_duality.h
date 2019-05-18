#pragma once

#include "base_utils.h"


using namespace nr;
using namespace nr::__internal;
using namespace testing;

void checkDuality(
    cl::CommandQueue q, 
    ScreenFromNDC screenFromNDC, NDCFromScreen ndcFromScreen, 
    const ScreenDimension& screenDim, const ScreenPosition& screen)
{
    cl_int err = CL_SUCCESS;
    Error error = Error::NO_ERROR;

    ScreenPosition screenResult;
    NDCPosition ndcResult;

    ndcFromScreen.global = cl::NDRange(1);
    ndcFromScreen.local  = cl::NDRange(1);
    ndcFromScreen.params.dimension = screenDim;
    ndcFromScreen.params.position = screen;
    ndcFromScreen.params.result = Buffer(CL_MEM_READ_WRITE, sizeof(ndcResult), &error);
    ASSERT_TRUE(isSuccess(error));

    ASSERT_TRUE(isSuccess(ndcFromScreen(q)));
    ASSERT_TRUE(isSuccess(q.enqueueReadBuffer(ndcFromScreen.params.result, CL_FALSE, 0, sizeof(ndcResult), &ndcResult)));
    ASSERT_TRUE(isSuccess(q.finish()));

    screenFromNDC.global = cl::NDRange(1);
    screenFromNDC.local  = cl::NDRange(1);
    screenFromNDC.params.dimension = screenDim;
    screenFromNDC.params.ndcPosition = ndcResult;
    screenFromNDC.params.result = cl::Buffer(CL_MEM_READ_WRITE, sizeof(screenResult), nullptr, &err);
    ASSERT_TRUE(isSuccess(err));

    ASSERT_TRUE(isSuccess(screenFromNDC(q)));
    ASSERT_TRUE(isSuccess(q.enqueueReadBuffer(ndcFromScreen.params.result, CL_FALSE, 0, sizeof(screenResult), &screenResult)));
    ASSERT_TRUE(isSuccess(q.finish()));

    ASSERT_EQ(screen, screenResult) << "Conversion: " << screen << " -> " << ndcResult << " -> " << screenResult << " | Screen dim: " << screenDim;
}


void checkDuality(
    cl::CommandQueue q, 
    ScreenFromNDC screenFromNDC, NDCFromScreen ndcFromScreen, 
    const ScreenDimension& screenDim, const NDCPosition& ndc)
{
    cl_int err = CL_SUCCESS;
    Error error = Error::NO_ERROR;

    ScreenPosition screenResult;
    NDCPosition ndcResult;

    screenFromNDC.global = cl::NDRange(1);
    screenFromNDC.local  = cl::NDRange(1);
    screenFromNDC.params.dimension = screenDim;
    screenFromNDC.params.ndcPosition = ndc;
    screenFromNDC.params.result = cl::Buffer(CL_MEM_READ_WRITE, sizeof(screenResult), nullptr, &err);
    ASSERT_TRUE(isSuccess(err));

    ASSERT_TRUE(isSuccess(screenFromNDC(q)));
    ASSERT_TRUE(isSuccess(q.enqueueReadBuffer(screenFromNDC.params.result, CL_FALSE, 0, sizeof(screenResult), &screenResult)));
    ASSERT_TRUE(isSuccess(q.finish()));

    ndcFromScreen.global = cl::NDRange(1);
    ndcFromScreen.local  = cl::NDRange(1);
    ndcFromScreen.params.dimension = screenDim;
    ndcFromScreen.params.position = screenResult;
    ndcFromScreen.params.result = Buffer(CL_MEM_READ_WRITE, sizeof(ndcResult), &error);
    ASSERT_TRUE(isSuccess(error));

    ASSERT_TRUE(isSuccess(ndcFromScreen(q)));
    ASSERT_TRUE(isSuccess(q.enqueueReadBuffer(ndcFromScreen.params.result, CL_FALSE, 0, sizeof(ndcResult), &ndcResult)));
    ASSERT_TRUE(isSuccess(q.finish()));

    ASSERT_TRUE(comparePoints(ndc, ndcResult, screenDim));
}


TEST(Base, ConversionDuality)
{
    cl_int err = CL_SUCCESS;
    auto q = cl::CommandQueue::getDefault();

    Module code(clcode::base, "-cl-std=CL2.0 -Werror -D RENDER_DIMENSION=3", &err);
    ASSERT_TRUE(isSuccess(err));

    ScreenFromNDC testeeNDC = code.makeKernel<ScreenFromNDCParams>("screen_from_ndc_kernel", &err);
    ASSERT_TRUE(isSuccess(err));

    NDCFromScreen testeeScreen = code.makeKernel<NDCFromScreenParams>("ndc_from_screen_test", &err);
    ASSERT_TRUE(isSuccess(err));

    checkDuality(q, testeeNDC, testeeScreen, ScreenDimension{10, 10}, NDCPosition{0.3, -0.44});
    checkDuality(q, testeeNDC, testeeScreen, ScreenDimension{1366, 123}, ScreenPosition{12, 122});
}