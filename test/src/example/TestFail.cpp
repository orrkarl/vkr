#include <iostream>

#include <catch2/catch_test_macros.hpp>

#include "../vk-util/VulkanContext.h"

#include <gpu/AllKernels.h>

TEST_CASE("Sanity", "[sanity]") {
    utils::VulkanContext ctx;
    REQUIRE(ctx.compute());

    auto setupModuleInfo = vkr::gpu::describeTriangleSetup();
    REQUIRE(setupModuleInfo.pCode[0] == 0xAABBCCDD);
}
