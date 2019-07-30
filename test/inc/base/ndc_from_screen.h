#pragma once

#include "base_utils.h"
#include <array>

using namespace nr;
using namespace nr::__internal;
using namespace testing;

void ndcFromScreenTestTemplate(NDCFromScreen kernel, CommandQueue q, const ScreenPosition& screen, const ScreenDimension& dim, const NDCPosition& expected)
{
    NDCPosition result;
    cl_status err = CL_SUCCESS;

    std::array<size_t, 1> global{1};
    std::array<size_t, 1> local{1};
    
    kernel.position  = screen;
    kernel.dimension = dim;
    kernel.result    = Buffer<NDCPosition>(defaultContext, CL_MEM_READ_WRITE, 1, &err);
    ASSERT_SUCCESS(err);

    ASSERT_SUCCESS(kernel.load());
    ASSERT_SUCCESS(q.enqueueKernelCommand<1>(kernel, global, local));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(kernel.result, false, 1, &result));
    ASSERT_SUCCESS(q.await());
    
    ASSERT_TRUE(comparePoints(expected, result, dim));
}

void checkConversionBounded(NDCFromScreen kernel, CommandQueue q, const ScreenPosition& screen, const ScreenDimension& dim)
{
    NDCPosition result;
    cl_status err = CL_SUCCESS;

    std::array<size_t, 1> global{1};
    std::array<size_t, 1> local{1};
    
    kernel.position  = screen;
    kernel.dimension = dim;
    kernel.result    = Buffer<NDCPosition>(defaultContext, CL_MEM_READ_WRITE, 1, &err);

    ASSERT_SUCCESS(kernel.load());
    ASSERT_SUCCESS(q.enqueueKernelCommand<1>(kernel, global, local));
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(kernel.result, false, 1, &result));
    ASSERT_SUCCESS(q.await());
    
    ASSERT_LE(result.x, 1.0f)  << "Converted X value too big! Result: "   << result << ", original: " << screen << " | Screen dim: " << dim;
    ASSERT_GE(result.x, -1.0f) << "Converted X value too small! Result: " << result << ", original: " << screen << " | Screen dim: " << dim;
    ASSERT_LE(result.y, 1.0f)  << "Converted Y value too big! Result: "   << result << ", original: " << screen << " | Screen dim: " << dim;
    ASSERT_GE(result.y, -1.0f) << "Converted Y value too small! Result: " << result << ", original: " << screen << " | Screen dim: " << dim;
}

TEST(Base, NDCFromScreen)
{
    cl_status err = CL_SUCCESS; 
 
    auto q = defaultCommandQueue;

    Module base(defaultContext, clcode::base, &err);
    ASSERT_SUCCESS(err);

    ASSERT_SUCCESS(base.build(defaultDevice, Module::Options{Module::_3D}));

    auto ndc_from_screen = NDCFromScreen(base, &err);
    ASSERT_SUCCESS(err);

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