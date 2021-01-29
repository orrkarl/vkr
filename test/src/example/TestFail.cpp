#include <iostream>

#include <catch2/catch_test_macros.hpp>

#include <gpu/host-interface/main/TriangleSetupAPI.h>
#include <gpu/host-interface/tests/setup/ClippingAPI.h>

#include "../vk-util/CommonVulkanResoruces.h"
#include "../vk-util/ManagedVulkanResource.h"
#include "../vk-util/VulkanContext.h"

TEST_CASE("Sanity", "[sanity]") {
    utils::VulkanContext ctx;
    REQUIRE(ctx.compute());

    {
        utils::ManagedVulkanResource<vkr::gpu::TriangleSetupAPI> triangleSetup(ctx.device().device, nullptr);

        VkPipeline triangleSetupStage;
        auto pipelineDesc = triangleSetup->describeRunner();
        REQUIRE(vkCreateComputePipelines(
                    ctx.device().device, VK_NULL_HANDLE, 1, &pipelineDesc, nullptr, &triangleSetupStage)
                == VK_SUCCESS);
        utils::ManagedVulkanResource<VkPipeline> triangleSetupStageGuard(
            ctx.device().device, std::move(triangleSetupStage), nullptr);
    }
    {
        utils::ManagedVulkanResource<vkr::gpu::tests::ClippingAPI> clipping(ctx.device().device, nullptr);
        VkPipeline clippingRunner;
        auto pipelineDesc = clipping->describeRunner();
        REQUIRE(vkCreateComputePipelines(
                    ctx.device().device, VK_NULL_HANDLE, 1, &pipelineDesc, nullptr, &clippingRunner)
                == VK_SUCCESS);
        utils::ManagedVulkanResource<VkPipeline> clippingRunnerGuard(
            ctx.device().device, std::move(clippingRunner), nullptr);
    }
}
