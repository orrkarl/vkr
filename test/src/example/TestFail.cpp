#include <iostream>

#include <catch2/catch.hpp>

#include <VkBootstrap.h>

#include <gpu/AllKernels.h>

vkb::Instance buildInstance() {
    vkb::InstanceBuilder builder;
    auto ret = builder.set_app_name("vkr-test-suite")
                   .request_validation_layers()
                   .require_api_version(1, 1, 0)
                   .use_default_debug_messenger()
                   .build();

    if (!ret) {
        auto err = static_cast<vkb::InstanceError>(ret.error().value());
        throw std::runtime_error(std::string("could not create instance: ") + vkb::to_string(err));
    }

    return ret.value();
}

TEST_CASE("Sanity", "[sanity]") {
    auto vkinst = buildInstance();
    auto setupModuleInfo = vkr::gpu::describeTriangleSetup();
    REQUIRE(vkinst.instance);
    REQUIRE(setupModuleInfo.pCode[0] == 0xAABBCCDD);
}
