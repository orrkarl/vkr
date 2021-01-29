#include <iostream>

#include <catch2/catch_test_macros.hpp>

#include <gpu/host-interface/main/TriangleSetupAPI.h>
#include <gpu/host-interface/tests/setup/ClippingAPI.h>

#include "../../utils/CommonVulkanResoruces.h"
#include "../../utils/ManagedVulkanResource.h"
#include "../../utils/VulkanContext.h"

using namespace utils;

TEST_CASE("Sanity", "[sanity]") {
    VulkanContext ctx;
    REQUIRE(ctx.computeQueue());

    {
        ManagedVulkanResource<vkr::gpu::TriangleSetupAPI> triangleSetup(ctx.rawDevice(), nullptr);
        auto triangleSetupStage = factory::computePipeline(ctx.rawDevice(), triangleSetup->describeRunner());
    }
    {
        ManagedVulkanResource<vkr::gpu::tests::ClippingAPI> clipping(ctx.rawDevice(), nullptr);
        auto clippingRunner = factory::computePipeline(ctx.rawDevice(), clipping->describeRunner());
    }
}
