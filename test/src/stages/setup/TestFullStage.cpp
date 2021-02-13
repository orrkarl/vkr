#include <catch2/catch_test_macros.hpp>

#include <gpu/host-interface/main/TriangleSetupAPI.h>

#include "../../utils/ManagedVulkanResource.h"
#include "../../utils/VulkanContext.h"

using namespace utils;

TEST_CASE("Triangle Setup full stage correctness", "[setup]") {
    VulkanContext ctx;
    ManagedVulkanResource<vkr::gpu::TriangleSetupAPI> triangleSetup(ctx.device(), nullptr);
    auto triangleSetupStage = ctx.device()
                                  .createComputePipelineUnique(nullptr, triangleSetup->describeRunner())
                                  .value;
}
