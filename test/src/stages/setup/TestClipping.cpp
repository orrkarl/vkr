#include <random>

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

    SECTION("clipping triangles inside the viewport does nothing") {
        constexpr size_t TRIANGLE_COUNT = 100;
        constexpr auto VERTEX_COUNT = 3 * TRIANGLE_COUNT;
        constexpr float MIN = std::numeric_limits<float>::min();
        constexpr float MAX = 1000; // TODO: think of a better way to generate floats

        std::array<vec4, VERTEX_COUNT> triangles;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> wDist(MIN, MAX);
        for (size_t i = 0; i < VERTEX_COUNT; ++i) {
            auto w = wDist(gen);
            std::uniform_real_distribution<float> pointDist(-w, w);
            triangles[i].x = pointDist(gen);
            triangles[i].y = pointDist(gen);
            triangles[i].z = pointDist(gen);
            triangles[i].w = w;
        }
    }
}
