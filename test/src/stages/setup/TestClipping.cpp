#include <iostream>
#include <random>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <gpu/host-interface/tests/setup/ClippingAPI.h>

#include "../SimpleVulkanFixture.h"

#include "../../utils/CommonDebugMessengers.h"
#include "../../utils/ManagedVulkanResource.h"
#include "../../utils/MemoryUtils.h"
#include "../../utils/VulkanContext.h"

using namespace utils;
using namespace vkr::gpu::tests;
using testing::Each;

class TriangleSetupClipping : public SimpleVulkanFixture {
protected:
    void SetUp() override {
        SimpleVulkanFixture::SetUp();
        m_clipping = std::make_unique<ManagedVulkanResource<ClippingAPI>>(context().device(), nullptr);
        m_clippingRunner = std::make_unique<vk::UniquePipeline>(
            context().device().createComputePipelineUnique(nullptr, clipping()->describeRunner()));
    }

    void TearDown() override {
        m_clippingRunner.reset();
        m_clipping.reset();
        SimpleVulkanFixture::TearDown();
    }

    ManagedVulkanResource<ClippingAPI>& clipping() {
        return *m_clipping;
    }

    vk::UniquePipeline& clippingRunner() {
        return *m_clippingRunner;
    }

private:
    std::unique_ptr<ManagedVulkanResource<ClippingAPI>> m_clipping;
    std::unique_ptr<vk::UniquePipeline> m_clippingRunner;
};

TEST_F(TriangleSetupClipping, TrianglesInViewport) {
    constexpr uint32_t TRIANGLE_COUNT = 100;
    constexpr auto VERTEX_COUNT = 3 * TRIANGLE_COUNT;
    constexpr auto MAX_CLIPPED_VERTECIES = 6 * TRIANGLE_COUNT;
    constexpr float MIN = std::numeric_limits<float>::min();
    constexpr float MAX = 1000; // TODO: think of a better way to generate floats

    constexpr std::pair<size_t, size_t> VERTECIES_SPACE = { 0, VERTEX_COUNT * sizeof(vec4) };
    constexpr std::pair<size_t, size_t> CLIPPED_VERTECIES_SPACE = { VERTECIES_SPACE.first + VERTECIES_SPACE.second,
                                                                    MAX_CLIPPED_VERTECIES * sizeof(vec4) };
    constexpr std::pair<size_t, size_t> CLIPPED_VERTECIES_COUNTS_SPACE = {
        CLIPPED_VERTECIES_SPACE.first + CLIPPED_VERTECIES_SPACE.second, TRIANGLE_COUNT * sizeof(u32)
    };

    auto vertexInput = context().device().createBufferUnique(
        vk::BufferCreateInfo(vk::BufferCreateFlags(), VERTECIES_SPACE.second, vk::BufferUsageFlagBits::eStorageBuffer));
    auto clippedVerteciesOutput = context().device().createBufferUnique(vk::BufferCreateInfo(
        vk::BufferCreateFlags(), CLIPPED_VERTECIES_SPACE.second, vk::BufferUsageFlagBits::eStorageBuffer));
    auto clipProductsCounts = context().device().createBufferUnique(vk::BufferCreateInfo(
        vk::BufferCreateFlags(), CLIPPED_VERTECIES_COUNTS_SPACE.second, vk::BufferUsageFlagBits::eStorageBuffer));

    auto memory = context().satisfyBuffersMemory({ *vertexInput, *clippedVerteciesOutput, *clipProductsCounts },
                                                 vk::MemoryPropertyFlagBits::eHostCoherent
                                                     | vk::MemoryPropertyFlagBits::eHostVisible);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<vec4> originalTriangles(VERTEX_COUNT);
    std::uniform_real_distribution<float> wDist(MIN, MAX);
    for (size_t i = 0; i < originalTriangles.size(); ++i) {
        auto w = wDist(gen);
        std::uniform_real_distribution<float> pointDist(-w, w);
        originalTriangles[i].x = pointDist(gen);
        originalTriangles[i].y = pointDist(gen);
        originalTriangles[i].z = pointDist(gen);
        originalTriangles[i].w = w;
    }
    {
        MappedMemoryGuard mapVertexInput(context().device(), *memory, VERTECIES_SPACE.first, VERTECIES_SPACE.second);
        std::copy(originalTriangles.cbegin(), originalTriangles.cend(), mapVertexInput.hostAddress<vec4>());
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
        VkDescriptorBufferInfo { *vertexInput, 0, VERTECIES_SPACE.second },
        VkDescriptorBufferInfo { *clippedVerteciesOutput, 0, CLIPPED_VERTECIES_SPACE.second },
        VkDescriptorBufferInfo { *clipProductsCounts, 0, CLIPPED_VERTECIES_COUNTS_SPACE.second }
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

    {
        MappedMemoryGuard mapClipCounts(
            context().device(), *memory, CLIPPED_VERTECIES_COUNTS_SPACE.first, CLIPPED_VERTECIES_COUNTS_SPACE.second);
        std::vector<u32> clipCounts(CLIPPED_VERTECIES_COUNTS_SPACE.second);
        auto mappedCountsPtr = mapClipCounts.hostAddress<u32>();
        std::copy(mappedCountsPtr, mappedCountsPtr + CLIPPED_VERTECIES_COUNTS_SPACE.second, clipCounts.begin());
        // Nothing was supposed to be clipped - 3 vertecies per triangle remain 3 vertecies
        EXPECT_THAT(clipCounts, Each(3));
    }

    {
        MappedMemoryGuard mapProducts(
            context().device(), *memory, CLIPPED_VERTECIES_SPACE.first, CLIPPED_VERTECIES_SPACE.second);
        std::vector<vec4> products(VERTEX_COUNT);
        auto mappedProductsPtr = mapProducts.hostAddress<vec4>();
        for (size_t i = 0; i < MAX_CLIPPED_VERTECIES / 6; ++i) {
            products[3 * i + 0] = mappedProductsPtr[6 * i + 0];
            products[3 * i + 1] = mappedProductsPtr[6 * i + 1];
            products[3 * i + 2] = mappedProductsPtr[6 * i + 2];
        }
        ASSERT_EQ(products, originalTriangles);
    }
}
