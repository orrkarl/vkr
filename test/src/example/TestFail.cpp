#include <iostream>

#include <catch2/catch_test_macros.hpp>

#include <VkBootstrap.h>

#include <gpu/AllKernels.h>

vkb::Instance buildInstance() {
    vkb::InstanceBuilder builder;
    auto ret = builder.set_app_name("vkr-test-suite")
                   .request_validation_layers()
                   .require_api_version(1, 1, 0)
                   .use_default_debug_messenger()
                   .set_headless()
                   .build();

    if (!ret) {
        throw std::runtime_error(std::string("could not create instance: ") + ret.error().message());
    }

    return ret.value();
}

vkb::PhysicalDevice pickPhysicalDevice(const vkb::Instance& inst) {
    vkb::PhysicalDeviceSelector physicalDev(inst);
    auto ret = physicalDev.set_minimum_version(1, 1)
                   .prefer_gpu_device_type(vkb::PreferredDeviceType::discrete)
                   .select();

    if (!ret) {
        throw std::runtime_error(std::string("could not pick physical device: ") + ret.error().message());
    }

    return ret.value();
}

vkb::Device buildLogicalDevice(const vkb::PhysicalDevice& pdev) {
    vkb::DeviceBuilder dev(pdev);
    auto ret = dev.build();

    if (!ret) {
        throw std::runtime_error(std::string("could not build logical device: ") + ret.error().message());
    }

    return ret.value();
}

TEST_CASE("Sanity", "[sanity]") {
    auto vkinst = buildInstance();

    auto pdev = pickPhysicalDevice(vkinst);
    std::cout << "[vkr-tests] picked physical device: " << pdev.properties.deviceName << std::endl;
    CAPTURE(pdev.properties.deviceName);

    auto dev = buildLogicalDevice(pdev);
    REQUIRE(dev.device);

    auto setupModuleInfo = vkr::gpu::describeTriangleSetup();
    REQUIRE(setupModuleInfo.pCode[0] == 0xAABBCCDD);
}
