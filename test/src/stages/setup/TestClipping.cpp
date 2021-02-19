#include <iostream>
#include <random>

#include <catch2/catch_test_macros.hpp>

#include <gpu/host-interface/tests/setup/ClippingAPI.h>

#include "../../utils/ManagedVulkanResource.h"
#include "../../utils/MemoryUtils.h"
#include "../../utils/VulkanContext.h"

#include "../../utils/CommonDebugMessengers.h"

using namespace utils;

TEST_CASE("Clipping correctness", "[setup]") {
    OstreamLoggingMessenger debug(std::cout, vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning);
    VulkanContext ctx({ debug });
    DebugMessengerRegisterGuard guard(ctx.instance().instance, ctx.dynamics(), debug);

    ManagedVulkanResource<vkr::gpu::tests::ClippingAPI> clipping(ctx.device(), nullptr);
    auto clippingRunner = ctx.device().createComputePipelineUnique(nullptr, clipping->describeRunner()).value;

    SECTION("clipping triangles inside the viewport does nothing") {
        constexpr uint32_t TRIANGLE_COUNT = 100;
        constexpr auto VERTEX_COUNT = 3 * TRIANGLE_COUNT;
        constexpr auto MAX_CLIPPED_VERTECIES = 6 * TRIANGLE_COUNT;
        constexpr float MIN = std::numeric_limits<float>::min();
        constexpr float MAX = 1000; // TODO: think of a better way to generate floats

        auto vertexInput = ctx.device().createBufferUnique(vk::BufferCreateInfo(
            vk::BufferCreateFlags(), VERTEX_COUNT * sizeof(vec4), vk::BufferUsageFlagBits::eStorageBuffer));
        auto clippedVerteciesOutput = ctx.device().createBufferUnique(vk::BufferCreateInfo(
            vk::BufferCreateFlags(), MAX_CLIPPED_VERTECIES * sizeof(vec4), vk::BufferUsageFlagBits::eStorageBuffer));
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

        auto clippingArgsLayout = clipping->describeArguments();
        vk::DescriptorPoolSize storageSetsCount(vk::DescriptorType::eStorageBuffer, 3);
        auto descPool = ctx.device().createDescriptorPoolUnique(
            { vk::DescriptorPoolCreateFlags(), 1, 1, &storageSetsCount });
        auto args = ctx.device().allocateDescriptorSets(
            { *descPool,
              clippingArgsLayout.size(),
              reinterpret_cast<vk::DescriptorSetLayout*>(clippingArgsLayout.data()) })[0];

        std::array<VkDescriptorBufferInfo, 3> bufferDescs {
            VkDescriptorBufferInfo { *vertexInput, 0, VERTEX_COUNT * sizeof(vec4) },
            VkDescriptorBufferInfo { *clippedVerteciesOutput, 0, MAX_CLIPPED_VERTECIES * sizeof(vec4) },
            VkDescriptorBufferInfo { *clipProductsCounts, 0, TRIANGLE_COUNT * sizeof(uint32_t) }
        };

        // each bufferDescs element has to live as long as the WriteDescriptorSet instances
        std::array<vk::WriteDescriptorSet, 3> updateSets {
            clipping->describeVerteciesUpdate(args, bufferDescs[0]),
            clipping->describeClippedVerteciesUpdate(args, bufferDescs[1]),
            clipping->describeClippedVertexCountsUpdate(args, bufferDescs[2])
        };

        ctx.device().updateDescriptorSets(updateSets, {});

        auto commandPool = ctx.createComputeCommnadPool();
        auto command = std::move(
            ctx.device().allocateCommandBuffers({ *commandPool, vk::CommandBufferLevel::ePrimary, 1 })[0]);

        command.begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
        command.bindPipeline(vk::PipelineBindPoint::eCompute, *clippingRunner);
        clipping->cmdUpdateTriangleCount(command, TRIANGLE_COUNT);
        command.bindDescriptorSets(vk::PipelineBindPoint::eCompute, clipping->runnerLayout(), 0, { args }, {});
        command.dispatch(
            (TRIANGLE_COUNT + clipping->dispatchGroupSizes()[0] - 1) / clipping->dispatchGroupSizes()[0], 1, 1);
        command.end();

        ctx.computeQueue().submit({ vk::SubmitInfo { 0, nullptr, nullptr, 1, &command } }, vk::Fence());
        ctx.computeQueue().waitIdle();
    }
}
