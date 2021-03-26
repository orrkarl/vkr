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

        auto clippingArgsLayout = buildVectorFrom<vk::DescriptorSetLayout>(m_clipping->describeArguments());
        m_descriptorPool = createDescriptorPool(1, { { vk::DescriptorType::eStorageBuffer, 3 } });
        m_argumentSets = buildArrayFrom<VkDescriptorSet, 1>(
            context().device().allocateDescriptorSets({ *m_descriptorPool, clippingArgsLayout }));
    }

    void TearDown() override {
        std::fill(m_argumentSets.begin(), m_argumentSets.end(), nullptr);
        m_descriptorPool.reset();
        m_clippingRunner.reset();
        m_clipping.reset();
        SimpleVulkanFixture::TearDown();
    }

    void updateAllSets(const std::array<VkDescriptorBufferInfo, 3>& descriptors) {
        // each bufferDescs element has to live as long as the WriteDescriptorSet instances
        std::array<vk::WriteDescriptorSet, 3> updateSets {
            m_clipping->describeVerteciesUpdate(m_argumentSets, descriptors[0]),
            m_clipping->describeClippedVerteciesUpdate(m_argumentSets, descriptors[1]),
            m_clipping->describeClippedVertexCountsUpdate(m_argumentSets, descriptors[2])
        };

        context().device().updateDescriptorSets(updateSets, {});
    }

    void dispatch(uint32_t triangleCount) {
        auto commandPool = context().createComputeCommnadPool();
        auto command = std::move(
            context().device().allocateCommandBuffers({ *commandPool, vk::CommandBufferLevel::ePrimary, 1 })[0]);

        command.begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
        command.bindPipeline(vk::PipelineBindPoint::eCompute, *m_clippingRunner);
        command.bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                   m_clipping->runnerLayout(),
                                   0,
                                   buildVectorFrom<vk::DescriptorSet>(m_argumentSets),
                                   {});
        m_clipping->cmdDispatch(command, triangleCount);
        command.end();

        context().computeQueue().submit({ vk::SubmitInfo { 0, nullptr, nullptr, 1, &command } }, vk::Fence());
        context().computeQueue().waitIdle();
    }

    ManagedVulkanResource<ClippingAPI> m_clipping;
    vk::UniquePipeline m_clippingRunner;
    vk::UniqueDescriptorPool m_descriptorPool;
    ClippingAPI::ArgumentSets m_argumentSets;
};

TEST_F(TriangleSetupClipping, TrianglesInViewport) {
    constexpr uint32_t TRIANGLE_COUNT = 100;
    constexpr auto VERTEX_COUNT = 3 * TRIANGLE_COUNT;
    constexpr auto MAX_CLIPPED_VERTECIES = 6 * TRIANGLE_COUNT;

    constexpr float MIN = std::numeric_limits<float>::min();
    constexpr float MAX = 1000; // TODO: think of a better way to generate floats

    TypedRegionDescriptor<vec4> vertexRegion { 0, VERTEX_COUNT };
    TypedRegionDescriptor<vec3> clippedVerteciesRegion { vertexRegion.end(), MAX_CLIPPED_VERTECIES };
    TypedRegionDescriptor<u32> clippedVerteciesCountsRegion { clippedVerteciesRegion.end(), TRIANGLE_COUNT };

    auto vertexInput = context().device().createBufferUnique(
        vk::BufferCreateInfo(vk::BufferCreateFlags(), vertexRegion.size(), vk::BufferUsageFlagBits::eStorageBuffer));
    auto clippedVerteciesOutput = context().device().createBufferUnique(vk::BufferCreateInfo(
        vk::BufferCreateFlags(), clippedVerteciesRegion.size(), vk::BufferUsageFlagBits::eStorageBuffer));
    auto clipProductsCounts = context().device().createBufferUnique(vk::BufferCreateInfo(
        vk::BufferCreateFlags(), clippedVerteciesCountsRegion.size(), vk::BufferUsageFlagBits::eStorageBuffer));

    auto memory = context().satisfyBuffersMemory({ *vertexInput, *clippedVerteciesOutput, *clipProductsCounts },
                                                 vk::MemoryPropertyFlagBits::eHostCoherent
                                                     | vk::MemoryPropertyFlagBits::eHostVisible);

    std::mt19937 gen(std::random_device {}());
    std::uniform_real_distribution<float> wDist(MIN, MAX);
    {
        MappedMemoryGuard mapVertexInput(context().device(), *memory, vertexRegion.offset(), vertexRegion.size());
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

    updateAllSets({ VkDescriptorBufferInfo { *vertexInput, 0, vertexRegion.size() },
                    VkDescriptorBufferInfo { *clippedVerteciesOutput, 0, clippedVerteciesRegion.size() },
                    VkDescriptorBufferInfo { *clipProductsCounts, 0, clippedVerteciesCountsRegion.size() } });

    dispatch(TRIANGLE_COUNT);

    std::vector<u32> clipCounts = readDeviceMemory<u32>(*memory, clippedVerteciesCountsRegion);

    // Since no triangle should be clipped, the last 3 vertecies should be garbage, and we discard them
    std::vector<std::array<vec3, 3>> barys = map(groupBy<6>(readDeviceMemory<vec3>(*memory, clippedVerteciesRegion)),
                                                 takeCountedFrom<3, std::array<vec3, 6>>);

    std::array<vec3, 3> expectedBarys { vec3 { 1.0f, 0.0f, 0.0f },
                                        vec3 { 0.0f, 1.0f, 0.0f },
                                        vec3 { 0.0f, 0.0f, 1.0f } };

    // Nothing was supposed to be clipped - 3 vertecies per triangle remain 3 vertecies
    EXPECT_THAT(clipCounts, Each(3));
    EXPECT_THAT(barys, Each(expectedBarys));
}
