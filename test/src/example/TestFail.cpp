#include <iostream>

#include <catch2/catch.hpp>
#include <gpu/AllKernels.h>

TEST_CASE("Sanity", "[sanity]") {
    auto simpleModuleInfo = vkr::gpu::describeSimple();
    REQUIRE(simpleModuleInfo.pCode[0] == 0xAABBCCDD);
}
