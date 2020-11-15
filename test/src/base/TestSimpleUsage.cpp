#include <gtest/gtest.h>

#include <base/Platform.h>

using namespace nr;

TEST(Example, Example) {
    auto platforms = base::Platform::getAvailablePlatforms();
    for (auto p : platforms) {
        auto devices = p.getDevicesByType(base::DeviceTypeFlag::All);
        for (auto d : devices) {
            std::cout << d.view().name() << std::endl;
        }
    }

    ASSERT_EQ(1, 1);
}
