#pragma once

#include "base_utils.h"

using namespace nr;
using namespace nr::__internal;

void screenFromNDCTestTemplate(ScreenFromNDC kernel, CommandQueue queue, const NDCPosition& ndc, const ScreenDimension& dim, const ScreenPosition& expected)
{
    cl_status err;

    std::array<NRuint, 1> global{1};
    std::array<NRuint, 1> local{1};
    
    kernel.ndcPosition = ndc;
    kernel.dimension   = dim;

    ASSERT_SUCCESS(kernel.load());
    ASSERT_SUCCESS(q.enqueueKernelCommand(kernel, global, local));
    ASSERT_SUCCESS(q.await());

    ScreenPosition result;
    ASSERT_SUCCESS(kernel.getResult(&result));

    ASSERT_EQ(expected, result) << "Screen from NDC didn't return correct values";
}

TEST(Base, ScreenFromNDC)
{
    cl_status err = CL_SUCCESS; 
 
    CommandQueue queue = CommandQueue::getDefault();

    Module base(clcode::base, Module::Options{Module::CL_VERSION_12, Module::WARNINGS_ARE_ERRORS, Module::_3D}, &err);
    ASSERT_SUCCESS(err);

    auto screen_from_ndc = ScreenFromNDC(base, &err);
    ASSERT_SUCCESS(err);

    screenFromNDCTestTemplate(screen_from_ndc, queue, NDCPosition{-1, -1}, ScreenDimension{100, 100}, ScreenPosition{0, 0});
    screenFromNDCTestTemplate(screen_from_ndc, queue, NDCPosition{0, 0}, ScreenDimension{100, 100}, ScreenPosition{50, 50});
    screenFromNDCTestTemplate(screen_from_ndc, queue, NDCPosition{0.99, 0.99}, ScreenDimension{100, 100}, ScreenPosition{99, 99});

    screenFromNDCTestTemplate(screen_from_ndc, queue, NDCPosition{-1, -1}, ScreenDimension{99, 99}, ScreenPosition{0, 0});
    screenFromNDCTestTemplate(screen_from_ndc, queue, NDCPosition{0, 0}, ScreenDimension{99, 99}, ScreenPosition{49, 49});
    screenFromNDCTestTemplate(screen_from_ndc, queue, NDCPosition{0.99, 0.99}, ScreenDimension{99, 99}, ScreenPosition{98, 98});
}