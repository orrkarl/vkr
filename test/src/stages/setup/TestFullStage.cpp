#include <iostream>

#include <catch2/catch_test_macros.hpp>

#include <gpu/host-interface/main/TriangleSetupAPI.h>

#include "../../utils/ManagedVulkanResource.h"
#include "../../utils/VulkanContext.h"

#include "../../utils/CommonDebugMessengers.h"

using namespace utils;

TEST_CASE("Triangle Setup full stage correctness", "[setup]") {
    OstreamLoggingMessenger debug(std::cout, vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning);
    VulkanContext ctx({ debug });
    DebugMessengerRegisterGuard guard(ctx.instance(), ctx.dynamics(), debug);

    ManagedVulkanResource<vkr::gpu::TriangleSetupAPI> triangleSetup(ctx.device(), nullptr);
    auto triangleSetupStage = ctx.device().createComputePipelineUnique(nullptr, triangleSetup->describeRunner()).value;
}
