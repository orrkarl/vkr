#pragma once

#include "base_utils.h"

#include <kernels/base.cl.h>

using namespace nr;
using namespace nr::detail;

void screenFromNDCTestTemplate(ScreenFromNDC& kernel, const CommandQueue& q, const NDCPosition& ndc, const ScreenDimension& dim, const ScreenPosition& expected)
{
	cl_status err = CL_SUCCESS;

	auto d_result = Buffer::make<ScreenPosition>(defaultContext, CL_MEM_WRITE_ONLY, 1, err);

	kernel.setExecutionRange(1);

	ASSERT_SUCCESS(kernel.setDimension(dim));
	ASSERT_SUCCESS(kernel.setPosition(ndc));
	ASSERT_SUCCESS(kernel.setResultBuffer(d_result));
    ASSERT_SUCCESS(q.enqueueDispatchCommand(kernel));
    ASSERT_SUCCESS(q.await());

    ScreenPosition result;
    ASSERT_SUCCESS(q.enqueueBufferReadCommand(d_result, true, 1, &result));

    ASSERT_EQ(expected, result) << "Screen from NDC didn't return correct values";
}

TEST(Base, ScreenFromNDC)   
{
    cl_status err = CL_SUCCESS; 

    Module base(defaultContext, clcode::base, err);
    ASSERT_SUCCESS(err);

    ASSERT_SUCCESS(base.build(defaultDevice, Module::Options{Module::CL_VERSION_12}));

    auto screen_from_ndc = ScreenFromNDC(base, err);
    ASSERT_SUCCESS(err);

    screenFromNDCTestTemplate(screen_from_ndc, defaultCommandQueue, NDCPosition{-1.0f, -1.0f}, ScreenDimension{100, 100}, ScreenPosition{0, 0});
    screenFromNDCTestTemplate(screen_from_ndc, defaultCommandQueue, NDCPosition{0.0f, 0.0f}, ScreenDimension{100, 100}, ScreenPosition{50, 50});
    screenFromNDCTestTemplate(screen_from_ndc, defaultCommandQueue, NDCPosition{0.99f, 0.99f}, ScreenDimension{100, 100}, ScreenPosition{99, 99});
											   
    screenFromNDCTestTemplate(screen_from_ndc, defaultCommandQueue, NDCPosition{-1.0f, -1.0f}, ScreenDimension{99, 99}, ScreenPosition{0, 0});
    screenFromNDCTestTemplate(screen_from_ndc, defaultCommandQueue, NDCPosition{0.0f, 0.0f}, ScreenDimension{99, 99}, ScreenPosition{49, 49});
    screenFromNDCTestTemplate(screen_from_ndc, defaultCommandQueue, NDCPosition{0.99f, 0.99f}, ScreenDimension{99, 99}, ScreenPosition{98, 98});
}