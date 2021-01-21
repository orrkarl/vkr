#include <iostream>

#include <catch2/catch_test_macros.hpp>

#include "../vk-util/VulkanContext.h"

#include <gpu/host-interface/TriangleSetupAPI.h>

TEST_CASE("Sanity", "[sanity]") {
    utils::VulkanContext ctx;
    REQUIRE(ctx.compute());

    vkr::gpu::TriangleSetupAPI triangleSetup;
    REQUIRE(triangleSetup.init(ctx.device().device, nullptr) == VK_SUCCESS);
    triangleSetup.destroy(ctx.device().device, nullptr);
}
