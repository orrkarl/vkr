#include <catch2/catch_test_macros.hpp>

#include <gpu/host-interface/tests/setup/ClippingAPI.h>

#include "../../utils/CommonVulkanResoruces.h"
#include "../../utils/ManagedVulkanResource.h"
#include "../../utils/VulkanContext.h"

using namespace utils;

TEST_CASE("Clipping correctness", "[setup]") {
    VulkanContext ctx;
    ManagedVulkanResource<vkr::gpu::tests::ClippingAPI> clipping(ctx.rawDevice(), nullptr);
    auto clippingRunner = factory::computePipeline(ctx.rawDevice(), clipping->describeRunner());
}
