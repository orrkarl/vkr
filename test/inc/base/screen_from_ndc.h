#pragma once

#include "base_utils.h"

#include <kernels/base.cl.h>

using namespace nr;
using namespace nr::__internal;

void screenFromNDCTestTemplate(ScreenFromNDC kernel, CommandQueue q, const NDCPosition& ndc, const ScreenDimension& dim, const ScreenPosition& expected)
{
	cl_status err = CL_SUCCESS;

    std::array<size_t, 1> global{1};
    std::array<size_t, 1> local{1};
    
    kernel.ndcPosition = ndc;
    kernel.dimension   = dim;
	kernel.result = Buffer<ScreenPosition>(CL_MEM_WRITE_ONLY, 1, &err);

    ASSERT_SUCCESS(kernel.load());
    ASSERT_SUCCESS(q.enqueueKernelCommand<1>(kernel, global, local));
    ASSERT_SUCCESS(q.await());

    ScreenPosition result;
    ASSERT_SUCCESS(kernel.getResult(&result));

    ASSERT_EQ(expected, result) << "Screen from NDC didn't return correct values";
}

TEST(Base, ScreenFromNDC)   
{
    cl_status err = CL_SUCCESS; 
 
    CommandQueue queue = CommandQueue::getDefault();

    Module base(clcode::base, &err);
    ASSERT_SUCCESS(err);

    ASSERT_SUCCESS(base.build(Module::Options{Module::CL_VERSION_12, Module::WARNINGS_ARE_ERRORS, Module::_3D}));

    auto screen_from_ndc = ScreenFromNDC(base, &err);
    ASSERT_SUCCESS(err);

    screenFromNDCTestTemplate(screen_from_ndc, queue, NDCPosition{-1.0f, -1.0f}, ScreenDimension{100, 100}, ScreenPosition{0, 0});
    screenFromNDCTestTemplate(screen_from_ndc, queue, NDCPosition{0.0f, 0.0f}, ScreenDimension{100, 100}, ScreenPosition{50, 50});
    screenFromNDCTestTemplate(screen_from_ndc, queue, NDCPosition{0.99f, 0.99f}, ScreenDimension{100, 100}, ScreenPosition{99, 99});

    screenFromNDCTestTemplate(screen_from_ndc, queue, NDCPosition{-1.0f, -1.0f}, ScreenDimension{99, 99}, ScreenPosition{0, 0});
    screenFromNDCTestTemplate(screen_from_ndc, queue, NDCPosition{0.0f, 0.0f}, ScreenDimension{99, 99}, ScreenPosition{49, 49});
    screenFromNDCTestTemplate(screen_from_ndc, queue, NDCPosition{0.99f, 0.99f}, ScreenDimension{99, 99}, ScreenPosition{98, 98});
}