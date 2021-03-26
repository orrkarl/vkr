#include <iostream>
#include <random>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <gpu/host-interface/tests/setup/ClippingAPI.h>

#include "../SimpleVulkanFixture.h"

#include "../../utils/CommonDebugMessengers.h"
#include "../../utils/IterUtils.h"
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
        m_clipping = ManagedVulkanResource<ClippingAPI>(context().device(), nullptr);
        m_clippingRunner = vk::UniquePipeline(
            context().device().createComputePipelineUnique(nullptr, m_clipping->describeRunner()).value);
    }

    void TearDown() override {
        m_clippingRunner.reset();
        m_clipping.reset();
        SimpleVulkanFixture::TearDown();
    }

    ManagedVulkanResource<ClippingAPI> m_clipping;
    vk::UniquePipeline m_clippingRunner;
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

    std::mt19937 gen(std::random_device {}());
    std::uniform_real_distribution<float> wDist(MIN, MAX);
    {
        MappedMemoryGuard mapVertexInput(context().device(), *memory, VERTECIES_SPACE.first, VERTECIES_SPACE.second);
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

    auto clippingArgsLayout = buildVectorFrom<vk::DescriptorSetLayout>(m_clipping->describeArguments());
    auto descPool = createDescriptorPool(1, { { vk::DescriptorType::eStorageBuffer, 3 } });
    auto args = buildArrayFrom<VkDescriptorSet, 1>(
        context().device().allocateDescriptorSets({ *descPool, clippingArgsLayout }));

    std::array<VkDescriptorBufferInfo, 3> bufferDescs {
        VkDescriptorBufferInfo { *vertexInput, 0, VERTECIES_SPACE.second },
        VkDescriptorBufferInfo { *clippedVerteciesOutput, 0, CLIPPED_VERTECIES_SPACE.second },
        VkDescriptorBufferInfo { *clipProductsCounts, 0, CLIPPED_VERTECIES_COUNTS_SPACE.second }
    };

    // each bufferDescs element has to live as long as the WriteDescriptorSet instances
    std::array<vk::WriteDescriptorSet, 3> updateSets { m_clipping->describeVerteciesUpdate(args, bufferDescs[0]),
                                                       m_clipping->describeClippedVerteciesUpdate(args, bufferDescs[1]),
                                                       m_clipping->describeClippedVertexCountsUpdate(args,
                                                                                                     bufferDescs[2]) };

    context().device().updateDescriptorSets(updateSets, {});

    auto commandPool = context().createComputeCommnadPool();
    auto command = std::move(
        context().device().allocateCommandBuffers({ *commandPool, vk::CommandBufferLevel::ePrimary, 1 })[0]);

    command.begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
    command.bindPipeline(vk::PipelineBindPoint::eCompute, *m_clippingRunner);
    m_clipping->cmdUpdateTriangleCount(command, TRIANGLE_COUNT);
    command.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute, m_clipping->runnerLayout(), 0, buildVectorFrom<vk::DescriptorSet>(args), {});
    command.dispatch(
        (TRIANGLE_COUNT + m_clipping->dispatchGroupSizes()[0] - 1) / m_clipping->dispatchGroupSizes()[0], 1, 1);
    command.end();

    context().computeQueue().submit({ vk::SubmitInfo { 0, nullptr, nullptr, 1, &command } }, vk::Fence());
    context().computeQueue().waitIdle();

    std::vector<u32> clipCounts(TRIANGLE_COUNT);
    std::vector<vec3> barys(VERTEX_COUNT);
    std::vector<vec3> expectedBarys(VERTEX_COUNT);

    {
        MappedMemoryGuard mapClipCounts(
            context().device(), *memory, CLIPPED_VERTECIES_COUNTS_SPACE.first, CLIPPED_VERTECIES_COUNTS_SPACE.second);
        auto mappedCountsPtr = mapClipCounts.hostAddress<u32>();
        std::copy(mappedCountsPtr, mappedCountsPtr + TRIANGLE_COUNT, clipCounts.begin());
    }

    {
        MappedMemoryGuard mapProducts(
            context().device(), *memory, CLIPPED_VERTECIES_SPACE.first, CLIPPED_VERTECIES_SPACE.second);
        for (size_t i = 0; i < barys.size(); i += 3) {
            barys[i + 0] = mapProducts.hostAddress<vec3>()[2 * i + 0];
            barys[i + 1] = mapProducts.hostAddress<vec3>()[2 * i + 1];
            barys[i + 2] = mapProducts.hostAddress<vec3>()[2 * i + 2];
            expectedBarys[i + 0] = vec3 { 1.0f, 0.0f, 0.0f };
            expectedBarys[i + 1] = vec3 { 0.0f, 1.0f, 0.0f };
            expectedBarys[i + 2] = vec3 { 0.0f, 0.0f, 1.0f };
        }
    }

    // Nothing was supposed to be clipped - 3 vertecies per triangle remain 3 vertecies
    EXPECT_THAT(clipCounts, Each(3));
    EXPECT_EQ(barys, expectedBarys);
}
