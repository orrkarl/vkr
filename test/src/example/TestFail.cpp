#include <iostream>

#include <catch2/catch.hpp>

#include <VkBootstrap.h>

#include <gpu/AllKernels.h>

TEST_CASE("Sanity", "[sanity]") {
    auto setupModuleInfo = vkr::gpu::describeTriangleSetup();
    REQUIRE(setupModuleInfo.pCode[0] == 0xAABBCCDD);
}
