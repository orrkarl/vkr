#pragma once

#include "base_utils.h"

using namespace nr;
using namespace nr::__internal;
using namespace testing;

void ndcFromScreenTestTemplate(Kernel<NDCFromScreenParams> kernel, cl::CommandQueue q, const ScreenPosition& screen, const ScreenDimension& dim, const NDCPosition& expected)
{
    NDCPosition result;
    cl_int err = CL_SUCCESS;

    kernel.global = cl::NDRange(1);
    kernel.local  = cl::NDRange(1);
    
    kernel.params.position  = screen;
    kernel.params.dimension = dim;
    kernel.params.result    = Buffer(CL_MEM_READ_WRITE, sizeof(result), &err);
    ASSERT_TRUE(isSuccess(err));

    ASSERT_TRUE(isSuccess(kernel(q)));
    ASSERT_TRUE(isSuccess(q.enqueueReadBuffer(kernel.params.result, CL_FALSE, 0, sizeof(NDCPosition), &result)));
    ASSERT_TRUE(isSuccess(q.finish()));
    
    ASSERT_TRUE(comparePoints(expected, result, dim));
}

void checkConversionBounded(Kernel<NDCFromScreenParams> kernel, cl::CommandQueue q, const ScreenPosition& screen, const ScreenDimension& dim)
{
    NDCPosition result;
    cl_int err = CL_SUCCESS;

    kernel.global = cl::NDRange(1);
    kernel.local  = cl::NDRange(1);
    
    kernel.params.position  = screen;
    kernel.params.dimension = dim;
    kernel.params.result    = Buffer(CL_MEM_READ_WRITE, sizeof(result), &err);

    ASSERT_TRUE(isSuccess(kernel(q)));
    ASSERT_TRUE(isSuccess(q.enqueueReadBuffer(kernel.params.result, CL_FALSE, 0, sizeof(NDCPosition), &result)));
    ASSERT_TRUE(isSuccess(q.finish()));
    
    ASSERT_LE(result.x, 1.0f)   << "Converted X value too big! Result: "   << result << ", original: " << screen << " | Screen dim: " << dim;
    ASSERT_GE(result.x, -1.0f)  << "Converted X value too small! Result: " << result << ", original: " << screen << " | Screen dim: " << dim;
    ASSERT_LE(result.y, 1.0f)   << "Converted Y value too big! Result: "   << result << ", original: " << screen << " | Screen dim: " << dim;
    ASSERT_GE(result.y, -1.0f)  << "Converted Y value too small! Result: " << result << ", original: " << screen << " | Screen dim: " << dim;
}

TEST(Base, NDCFromScreen)
{
    cl_int err = CL_SUCCESS; 
 
    cl::CommandQueue q = cl::CommandQueue::getDefault();

    Module base(clcode::base, Module::Options{Module::CL_VERSION_12, Module::WARNINGS_ARE_ERRORS, Module::_3D}, &err);
    ASSERT_TRUE(isSuccess(err));

    auto ndc_from_screen = base.makeKernel<NDCFromScreenParams>("ndc_from_screen_test", &err);
    ASSERT_TRUE(isSuccess(err));

    ndcFromScreenTestTemplate(ndc_from_screen, q, ScreenPosition{0, 0}, ScreenDimension{100, 100}, NDCPosition{-1, -1});
    ndcFromScreenTestTemplate(ndc_from_screen, q, ScreenPosition{49, 49}, ScreenDimension{100, 100}, NDCPosition{0, 0});
    ndcFromScreenTestTemplate(ndc_from_screen, q, ScreenPosition{99, 99}, ScreenDimension{100, 100}, NDCPosition{0.99, 0.99});
    ndcFromScreenTestTemplate(ndc_from_screen, q, ScreenPosition{0, 0}, ScreenDimension{99, 99}, NDCPosition{-0.99, -0.99});
    ndcFromScreenTestTemplate(ndc_from_screen, q, ScreenPosition{49, 49}, ScreenDimension{99, 99}, NDCPosition{0, 0});
    ndcFromScreenTestTemplate(ndc_from_screen, q, ScreenPosition{98, 98}, ScreenDimension{99, 99}, NDCPosition{0.99, 0.99});
    
    checkConversionBounded(ndc_from_screen, q, ScreenPosition{5, 9}, ScreenDimension{20, 10});
    checkConversionBounded(ndc_from_screen, q, ScreenPosition{40, 9}, ScreenDimension{400, 10});
    checkConversionBounded(ndc_from_screen, q, ScreenPosition{1919, 1079}, ScreenDimension{1920, 1080});
}