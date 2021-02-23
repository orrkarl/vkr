#include <iostream>
#include <random>

#include <gtest/gtest.h>

#include <gpu/host-interface/tests/setup/ClippingAPI.h>

#include "../SimpleVulkanFixture.h"

#include "../../utils/CommonDebugMessengers.h"
#include "../../utils/ManagedVulkanResource.h"
#include "../../utils/MemoryUtils.h"
#include "../../utils/VulkanContext.h"

using namespace utils;

class TriangleSetupClipping : public SimpleVulkanFixture {
protected:
    void SetUp() override {
        SimpleVulkanFixture::SetUp();
        m_clipping = std::make_unique<ManagedVulkanResource<vkr::gpu::tests::ClippingAPI>>(context().device(), nullptr);
        m_clippingRunner = std::make_unique<vk::UniquePipeline>(
            context().device().createComputePipelineUnique(nullptr, clipping()->describeRunner()));
    }

    void TearDown() override {
        m_clippingRunner.reset();
        m_clipping.reset();
        SimpleVulkanFixture::TearDown();
    }

    ManagedVulkanResource<vkr::gpu::tests::ClippingAPI>& clipping() {
        return *m_clipping;
    }

    vk::UniquePipeline& clippingRunner() {
        return *m_clippingRunner;
    }

private:
    std::unique_ptr<ManagedVulkanResource<vkr::gpu::tests::ClippingAPI>> m_clipping;
    std::unique_ptr<vk::UniquePipeline> m_clippingRunner;
};

TEST_F(TriangleSetupClipping, TrianglesInViewport) {
    constexpr uint32_t TRIANGLE_COUNT = 100;
    constexpr auto VERTEX_COUNT = 3 * TRIANGLE_COUNT;
    constexpr auto MAX_CLIPPED_VERTECIES = 6 * TRIANGLE_COUNT;
    constexpr float MIN = std::numeric_limits<float>::min();
    constexpr float MAX = 1000; // TODO: think of a better way to generate floats

    auto vertexInput = context().device().createBufferUnique(vk::BufferCreateInfo(
        vk::BufferCreateFlags(), VERTEX_COUNT * sizeof(vec4), vk::BufferUsageFlagBits::eStorageBuffer));
    auto clippedVerteciesOutput = context().device().createBufferUnique(vk::BufferCreateInfo(
        vk::BufferCreateFlags(), MAX_CLIPPED_VERTECIES * sizeof(vec4), vk::BufferUsageFlagBits::eStorageBuffer));
    auto clipProductsCounts = context().device().createBufferUnique(vk::BufferCreateInfo(
        vk::BufferCreateFlags(), TRIANGLE_COUNT * sizeof(u32), vk::BufferUsageFlagBits::eStorageBuffer));

    auto memory = context().satisfyBuffersMemory({ *vertexInput, *clippedVerteciesOutput, *clipProductsCounts },
                                                 vk::MemoryPropertyFlagBits::eHostCoherent
                                                     | vk::MemoryPropertyFlagBits::eHostVisible);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> wDist(MIN, MAX);
    {
        vk::Device devView = context().device();
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

    auto clippingArgsLayout = clipping()->describeArguments();
    vk::DescriptorPoolSize storageSetsCount(vk::DescriptorType::eStorageBuffer, 3);
    auto descPool = context().device().createDescriptorPoolUnique(
        { vk::DescriptorPoolCreateFlags(), 1, 1, &storageSetsCount });
    auto args = context().device().allocateDescriptorSets(
        { *descPool,
          clippingArgsLayout.size(),
          reinterpret_cast<vk::DescriptorSetLayout*>(clippingArgsLayout.data()) })[0];

    std::array<VkDescriptorBufferInfo, 3> bufferDescs {
        VkDescriptorBufferInfo { *vertexInput, 0, VERTEX_COUNT * sizeof(vec4) },
        VkDescriptorBufferInfo { *clippedVerteciesOutput, 0, MAX_CLIPPED_VERTECIES * sizeof(vec4) },
        VkDescriptorBufferInfo { *clipProductsCounts, 0, TRIANGLE_COUNT * sizeof(uint32_t) }
    };

    // each bufferDescs element has to live as long as the WriteDescriptorSet instances
    std::array<vk::WriteDescriptorSet, 3> updateSets { clipping()->describeVerteciesUpdate(args, bufferDescs[0]),
                                                       clipping()->describeClippedVerteciesUpdate(args, bufferDescs[1]),
                                                       clipping()->describeClippedVertexCountsUpdate(args,
                                                                                                     bufferDescs[2]) };

    context().device().updateDescriptorSets(updateSets, {});

    auto commandPool = context().createComputeCommnadPool();
    auto command = std::move(
        context().device().allocateCommandBuffers({ *commandPool, vk::CommandBufferLevel::ePrimary, 1 })[0]);

    command.begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
    command.bindPipeline(vk::PipelineBindPoint::eCompute, *clippingRunner());
    clipping()->cmdUpdateTriangleCount(command, TRIANGLE_COUNT);
    command.bindDescriptorSets(vk::PipelineBindPoint::eCompute, clipping()->runnerLayout(), 0, { args }, {});
    command.dispatch(
        (TRIANGLE_COUNT + clipping()->dispatchGroupSizes()[0] - 1) / clipping()->dispatchGroupSizes()[0], 1, 1);
    command.end();

    context().computeQueue().submit({ vk::SubmitInfo { 0, nullptr, nullptr, 1, &command } }, vk::Fence());
    context().computeQueue().waitIdle();
}
