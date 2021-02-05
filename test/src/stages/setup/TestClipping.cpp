#include <random>

#include <catch2/catch_test_macros.hpp>

#include <gpu/host-interface/tests/setup/ClippingAPI.h>

#include "../../utils/ManagedVulkanResource.h"
#include "../../utils/MemoryUtils.h"
#include "../../utils/VulkanContext.h"

using namespace utils;

TEST_CASE("Clipping correctness", "[setup]") {
    VulkanContext ctx;
    ManagedVulkanResource<vkr::gpu::tests::ClippingAPI> clipping(ctx.device(), nullptr);
    auto clippingRunner = ctx.device().createComputePipelineUnique(nullptr, clipping->describeRunner());

    SECTION("clipping triangles inside the viewport does nothing") {
        constexpr size_t TRIANGLE_COUNT = 100;
        constexpr auto VERTEX_COUNT = 3 * TRIANGLE_COUNT;
        constexpr auto MAX_CLIPPED_VERTECIES = 6 * TRIANGLE_COUNT;
        constexpr float MIN = std::numeric_limits<float>::min();
        constexpr float MAX = 1000; // TODO: think of a better way to generate floats

        auto vertexInput = ctx.device().createBufferUnique(vk::BufferCreateInfo(
            vk::BufferCreateFlags(), VERTEX_COUNT * sizeof(vec4), vk::BufferUsageFlagBits::eStorageBuffer));
        auto clippedVerteciesOutput = ctx.device().createBufferUnique(
            vk::BufferCreateInfo(vk::BufferCreateFlags(),
                                 MAX_CLIPPED_VERTECIES * sizeof(vec4),
                                 vk::BufferUsageFlagBits::eStorageBuffer));
        auto clipProductsCounts = ctx.device().createBufferUnique(vk::BufferCreateInfo(
            vk::BufferCreateFlags(), TRIANGLE_COUNT * sizeof(u32), vk::BufferUsageFlagBits::eStorageBuffer));

        auto memory = ctx.satisfyBuffersMemory({ *vertexInput, *clippedVerteciesOutput, *clipProductsCounts },
                                               vk::MemoryPropertyFlagBits::eHostCoherent
                                                   | vk::MemoryPropertyFlagBits::eHostVisible);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> wDist(MIN, MAX);
        {
            vk::Device devView = ctx.device();
            MappedMemoryGuard mapVertexInput(devView, *memory, VERTEX_COUNT * sizeof(vec4));
            auto triangles = mapVertexInput.hostAddress<vec4>();
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
}
