#include <iostream>

#include <catch2/catch_test_macros.hpp>

#include <VkBootstrap.h>

#include <gpu/AllKernels.h>

namespace vkb::detail {
VkQueue get_queue(VkDevice device, uint32_t family);
}

vkb::Instance buildInstance() {
    vkb::InstanceBuilder builder;
    auto ret = builder.set_app_name("vkr-test-suite")
                   .request_validation_layers()
                   .require_api_version(1, 1, 0)
                   .use_default_debug_messenger()
                   .set_headless()
                   .build();

    if (!ret) {
        throw std::runtime_error("could not create instance: " + ret.error().message());
    }

    return ret.value();
}

vkb::PhysicalDevice pickPhysicalDevice(const vkb::Instance& inst) {
    vkb::PhysicalDeviceSelector physicalDev(inst);

    VkPhysicalDeviceFeatures requiredFeatures {};
    requiredFeatures.shaderInt16 = VK_TRUE;
    requiredFeatures.shaderInt64 = VK_TRUE;

    auto ret = physicalDev.set_minimum_version(1, 1)
                   .prefer_gpu_device_type(vkb::PreferredDeviceType::discrete)
                   .set_required_features(requiredFeatures)
                   .select();

    if (!ret) {
        throw std::runtime_error("could not pick physical device: " + ret.error().message());
    }

    return ret.value();
}

vkb::Device buildLogicalDevice(const vkb::PhysicalDevice& pdev) {
    vkb::DeviceBuilder dev(pdev);
    auto ret = dev.build();

    if (!ret) {
        throw std::runtime_error("could not build logical device: " + ret.error().message());
    }

    return ret.value();
}

VkQueue acquireComputeQueue(const vkb::Device dev) {
    auto computeQueueFamily = std::find_if(
        dev.queue_families.cbegin(), dev.queue_families.cend(),
        [](const auto& family) { return (family.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0; });
    if (computeQueueFamily == dev.queue_families.cend()) {
        throw std::runtime_error("could not acquire compute queue: "
                                 + vkb::make_error_code(vkb::QueueError::compute_unavailable).message());
    }

    return vkb::detail::get_queue(dev.device,
                                  static_cast<uint32_t>(computeQueueFamily - dev.queue_families.cbegin()));
}

TEST_CASE("Sanity", "[sanity]") {
    auto vkinst = buildInstance();

    auto pdev = pickPhysicalDevice(vkinst);
    std::cout << "[vkr-tests] picked physical device: " << pdev.properties.deviceName << std::endl;

    auto dev = buildLogicalDevice(pdev);
    REQUIRE(dev.device);

    auto compq = acquireComputeQueue(dev);
    REQUIRE(compq);

    auto setupModuleInfo = vkr::gpu::describeTriangleSetup();
    REQUIRE(setupModuleInfo.pCode[0] == 0xAABBCCDD);
}
