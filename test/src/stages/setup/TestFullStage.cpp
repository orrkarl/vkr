#include <iostream>

#include <catch2/catch_test_macros.hpp>

#include <gpu/host-interface/main/TriangleSetupAPI.h>

#include "../../utils/ManagedVulkanResource.h"
#include "../../utils/VulkanContext.h"

#include "../../utils/CommonDebugMessengers.h"

using namespace utils;

TEST_CASE("Triangle Setup full stage correctness", "[setup]") {
    OstreamLoggingMessenger realtimeMessenger(std::cout, vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning);
    SeverityCountMessenger validationFailures(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                                              | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
    VulkanContext ctx({ realtimeMessenger, validationFailures });
    DebugMessengerRegisterGuard registerRealtime(ctx.instance(), ctx.dynamics(), realtimeMessenger);
    DebugMessengerRegisterGuard registerFailCount(ctx.instance(), ctx.dynamics(), validationFailures);

    ManagedVulkanResource<vkr::gpu::TriangleSetupAPI> triangleSetup(ctx.device(), nullptr);
    auto triangleSetupStage = ctx.device().createComputePipelineUnique(nullptr, triangleSetup->describeRunner()).value;

    REQUIRE(validationFailures.count() == 0);
}
