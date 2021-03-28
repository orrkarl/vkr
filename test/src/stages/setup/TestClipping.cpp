#include <algorithm>
#include <chrono>
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

constexpr uint32_t TEST_TRIANGLE_COUNT = 100;

class TriangleSetupClipping : public SimpleVulkanFixture {
public:
    TriangleSetupClipping()
        : SimpleVulkanFixture()
        , m_triangleCount(TEST_TRIANGLE_COUNT)
        , m_vertexCount(3 * m_triangleCount)
        , m_clippedVerteciesCount(6 * m_triangleCount)
        , m_inputVerteciesRegion(0, m_vertexCount)
        , m_outputClippedVerteciesRegion(m_inputVerteciesRegion.end(), m_clippedVerteciesCount)
        , m_outputClippedVertexCountsRegion(m_outputClippedVerteciesRegion.end(), m_triangleCount) {
    }

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

        m_argumentsBuffer = context().device().createBufferUnique({ vk::BufferCreateFlags(),
                                                                    m_inputVerteciesRegion.size()
                                                                        + m_outputClippedVerteciesRegion.size()
                                                                        + m_outputClippedVertexCountsRegion.size(),
                                                                    vk::BufferUsageFlagBits::eStorageBuffer });

        m_argumentsMemory = context().allocate(context().device().getBufferMemoryRequirements(*m_argumentsBuffer),
                                               vk::MemoryPropertyFlagBits::eHostCoherent
                                                   | vk::MemoryPropertyFlagBits::eHostVisible);
        context().device().bindBufferMemory(*m_argumentsBuffer, *m_argumentsMemory, 0);

        std::array<VkDescriptorBufferInfo, 3> descriptors {
            VkDescriptorBufferInfo { *m_argumentsBuffer, 0, m_inputVerteciesRegion.size() },
            VkDescriptorBufferInfo {
                *m_argumentsBuffer, m_inputVerteciesRegion.end(), m_outputClippedVerteciesRegion.size() },
            VkDescriptorBufferInfo {
                *m_argumentsBuffer, m_outputClippedVerteciesRegion.end(), m_outputClippedVertexCountsRegion.size() }
        };

        // each bufferDescs element has to live as long as the WriteDescriptorSet instances
        std::array<vk::WriteDescriptorSet, 3> updateSets {
            m_clipping->describeVerteciesUpdate(m_argumentSets, descriptors[0]),
            m_clipping->describeClippedVerteciesUpdate(m_argumentSets, descriptors[1]),
            m_clipping->describeClippedVertexCountsUpdate(m_argumentSets, descriptors[2])
        };

        context().device().updateDescriptorSets(updateSets, {});
    }

    void TearDown() override {
        m_argumentsMemory.reset();
        m_argumentsBuffer.reset();
        std::fill(m_argumentSets.begin(), m_argumentSets.end(), nullptr);
        m_descriptorPool.reset();
        m_clippingRunner.reset();
        m_clipping.reset();
        SimpleVulkanFixture::TearDown();
    }

    void dispatch() {
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
        m_clipping->cmdDispatch(command, m_triangleCount);
        command.end();

        auto start = std::chrono::steady_clock::now();
        context().computeQueue().submit({ vk::SubmitInfo { 0, nullptr, nullptr, 1, &command } }, vk::Fence());
        context().computeQueue().waitIdle();
        auto end = std::chrono::steady_clock::now();

        std::cout << "Command Dispatch Took: "
                  << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "us" << std::endl;
    }

    vk::DeviceMemory argumentsMemory() {
        return *m_argumentsMemory;
    }

    const uint32_t m_triangleCount;
    const uint32_t m_vertexCount;
    const uint32_t m_clippedVerteciesCount;
    const TypedRegionDescriptor<vec4> m_inputVerteciesRegion;
    const TypedRegionDescriptor<vec3> m_outputClippedVerteciesRegion;
    const TypedRegionDescriptor<u32> m_outputClippedVertexCountsRegion;

    ManagedVulkanResource<ClippingAPI> m_clipping;
    vk::UniquePipeline m_clippingRunner;
    vk::UniqueDescriptorPool m_descriptorPool;

private:
    ClippingAPI::ArgumentSets m_argumentSets;
    vk::UniqueBuffer m_argumentsBuffer;
    vk::UniqueDeviceMemory m_argumentsMemory;
};

vec4 generateInsideViewport(std::mt19937& generator, std::uniform_real_distribution<float>& wDist) {
    const float w = wDist(generator);
    std::uniform_real_distribution<float> pointDist(-w, w);
    return { pointDist(generator), pointDist(generator), pointDist(generator), w };
}

vec4 combine(const vec3& barys, const std::array<vec4, 3>& base) {
    return barys.x * base[0] + barys.y * base[1] + barys.z * base[2];
}

MATCHER(ValidBarycentricCoordinates, "") {
    return vec3(0) <= arg && arg <= vec3(1) && (std::abs(arg.x + arg.y + arg.z - 1) <= 1e-6);
}

TEST_F(TriangleSetupClipping, TrianglesOutsideViewport) {
    constexpr float MIN = std::numeric_limits<float>::min();
    constexpr float MAX = 1000; // TODO: think of a better way to generate floats

    {
        std::mt19937 gen(std::random_device {}());
        std::uniform_real_distribution<float> wDist(MIN, MAX);
        std::array<uint32_t, 3> indices { 0, 1, 2 };
        float currentSign = 1;

        MappedMemoryGuard mapVertexInput(
            context().device(), argumentsMemory(), m_inputVerteciesRegion.offset(), m_inputVerteciesRegion.size());
        auto triangles = mapVertexInput.hostAddress<vec4>();
        for (size_t t = 0; t < m_triangleCount; ++t) {
            for (size_t i = 0; i < 3; ++i) {
                auto w = wDist(gen);
                std::uniform_real_distribution<float> pointDist(-w, w);
                triangles[3 * t + i].data[indices[0]] = 1.1 * std::copysignf(w, currentSign)
                    + std::copysignf(pointDist(gen), currentSign);
                triangles[3 * t + i].data[indices[1]] = pointDist(gen);
                triangles[3 * t + i].data[indices[2]] = pointDist(gen);
                triangles[3 * t + i].data[3] = w;
            }

            std::rotate(indices.begin(), indices.begin() + 1, indices.end());
            if (t % 3 == 0) {
                currentSign *= -1;
            }
        }
    }

    dispatch();

    std::vector<u32> clipCounts = readDeviceMemory<u32>(argumentsMemory(), m_outputClippedVertexCountsRegion);

    // All triangles should be culled
    EXPECT_THAT(clipCounts, Each(0));
}

TEST_F(TriangleSetupClipping, TrianglesInViewport) {
    constexpr float MIN = std::numeric_limits<float>::min();
    constexpr float MAX = 1000; // TODO: think of a better way to generate floats

    {
        std::mt19937 gen(std::random_device {}());
        std::uniform_real_distribution<float> wDist(MIN, MAX);
        MappedMemoryGuard mapVertexInput(
            context().device(), argumentsMemory(), m_inputVerteciesRegion.offset(), m_inputVerteciesRegion.size());

        auto triangles = mapVertexInput.hostAddress<vec4>();
        for (size_t i = 0; i < m_vertexCount; ++i) {
            triangles[i] = generateInsideViewport(gen, wDist);
        }
    }

    dispatch();

    std::vector<u32> clipCounts = readDeviceMemory<u32>(argumentsMemory(), m_outputClippedVertexCountsRegion);

    // Since no triangle should be clipped, the last 3 vertecies should be garbage, and we discard them
    std::vector<std::array<vec3, 3>> barys = map(
        groupBy<6>(readDeviceMemory<vec3>(argumentsMemory(), m_outputClippedVerteciesRegion)),
        takeCountedFrom<3, std::array<vec3, 6>>);

    std::array<vec3, 3> expectedBarys { vec3 { 1.0f, 0.0f, 0.0f },
                                        vec3 { 0.0f, 1.0f, 0.0f },
                                        vec3 { 0.0f, 0.0f, 1.0f } };

    // Nothing was supposed to be clipped - 3 vertecies per triangle remain 3 vertecies
    EXPECT_THAT(clipCounts, Each(3));
    EXPECT_THAT(barys, Each(expectedBarys));
}

TEST_F(TriangleSetupClipping, SingleClipProduct) {
    constexpr float MIN = std::numeric_limits<float>::min();
    constexpr float MAX = 1000; // TODO: think of a better way to generate floats

    std::vector<std::array<vec4, 3>> trianglesRecord;
    {
        std::ofstream trianglesOut("triangles.log");
        std::mt19937 gen(std::random_device {}());
        std::array<uint32_t, 3> vertexPermutation { 0, 1, 2 };
        std::uniform_real_distribution<float> wDist(MIN, MAX);
        std::uniform_int_distribution axisDist(0, 2);
        std::uniform_int_distribution frontBackDist(0, 1);

        MappedMemoryGuard mapVertexInput(
            context().device(), argumentsMemory(), m_inputVerteciesRegion.offset(), m_inputVerteciesRegion.size());

        auto triangles = mapVertexInput.hostAddress<vec4>();
        for (size_t t = 0; t < m_triangleCount; ++t) {
            std::array<vec4, 3> currentTriangle;

            auto tmpV1 = generateInsideViewport(gen, wDist);
            auto tmpV2 = generateInsideViewport(gen, wDist);

            vec3 offset;
            offset.data[axisDist(gen)] = frontBackDist(gen) * 2 - 1;

            currentTriangle[vertexPermutation[0]] = generateInsideViewport(gen, wDist);
            currentTriangle[vertexPermutation[1]] = vec4(0.5f * vec3(tmpV1) + 1.5f * tmpV1.w * offset, tmpV1.w);
            currentTriangle[vertexPermutation[2]] = vec4(0.5f * vec3(tmpV2) + 1.5f * tmpV2.w * offset, tmpV2.w);

            triangles[3 * t + 0] = currentTriangle[0];
            triangles[3 * t + 1] = currentTriangle[1];
            triangles[3 * t + 2] = currentTriangle[2];

            trianglesOut << "Triangle " << t << ":" << std::endl;
            trianglesOut << "\t" << currentTriangle[0] << std::endl;
            trianglesOut << "\t" << currentTriangle[1] << std::endl;
            trianglesOut << "\t" << currentTriangle[2] << std::endl;
            trianglesOut << std::endl;

            trianglesRecord.push_back(currentTriangle);
            std::next_permutation(vertexPermutation.begin(), vertexPermutation.end());
        }
    }

    dispatch();

    std::vector<u32> clipCounts = readDeviceMemory<u32>(argumentsMemory(), m_outputClippedVertexCountsRegion);

    std::vector<vec3> rawBarys = readDeviceMemory<vec3>(argumentsMemory(), m_outputClippedVerteciesRegion);

    {
        std::ofstream clipProductsLog("clipped.log");
        for (size_t i = 0; i < rawBarys.size() / 6; ++i) {
            clipProductsLog << "Output Triangle " << i << ":" << std::endl;
            clipProductsLog << "\tbarys:" << std::endl;
            for (size_t v = 0; v < clipCounts[i]; ++v) {
                clipProductsLog << "\t\t" << rawBarys[6 * i + v] << std::endl;
            }
            for (size_t t = 1; t < clipCounts[i] - 1; ++t) {
                clipProductsLog << "\tsub-triangle " << t << std::endl;
                clipProductsLog << "\t\t" << combine(rawBarys[6 * i + 0], trianglesRecord[i]) << std::endl;
                clipProductsLog << "\t\t" << combine(rawBarys[6 * i + t], trianglesRecord[i]) << std::endl;
                clipProductsLog << "\t\t" << combine(rawBarys[6 * i + t + 1], trianglesRecord[i]) << std::endl;
            }
        }
    }

    EXPECT_THAT(clipCounts, Each(3));
    EXPECT_THAT(rawBarys, Each(ValidBarycentricCoordinates()));
}
