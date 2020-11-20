#include <iostream>

#include <catch2/catch.hpp>

#include <base/Platform.h>

using namespace nr;

TEST_CASE("Example", "[example]") {
    auto platforms = base::Platform::getAvailablePlatforms();
    for (auto p : platforms) {
        auto devices = p.getDevicesByType(base::DeviceTypeFlag::All);
        for (auto d : devices) {
            std::cout << d.name() << std::endl;
        }
    }

    SECTION("SUCCESS") {
        REQUIRE(1 == 1);
    }
    SECTION("FAILURE") {
        REQUIRE(1 == 2);
    }
}
