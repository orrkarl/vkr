#pragma once

#include <array>
#include <utility>

#include <vulkan/vulkan.h>

namespace vkr {
namespace gpu {

class TriangleSetupAPI {
public:
    static constexpr size_t ARGUMENTS_SET_COUNT = 2;
    using ArgumentsLayout = std::array<VkDescriptorSetLayout, ARGUMENTS_SET_COUNT>;
    using Arguments = std::array<VkDescriptorSet, ARGUMENTS_SET_COUNT>;

    explicit TriangleSetupAPI();
    VkResult init(VkDevice dev, const VkAllocationCallbacks* allocator);
    void destroy(VkDevice dev, const VkAllocationCallbacks* allocator);

    ArgumentsLayout describeArguments();
    VkComputePipelineCreateInfo describeStage();

    VkWriteDescriptorSet describeRasterConfigUpdate(const Arguments& args, const VkDescriptorBufferInfo& rasterConfigBuffer);
    VkWriteDescriptorSet describeVertexUpdate(const Arguments& args, const VkDescriptorBufferInfo& vertexBuffer);
    VkWriteDescriptorSet describeColorUpdate(const Arguments& args, const VkDescriptorBufferInfo& colorBuffer);
    VkWriteDescriptorSet describeTriangleBlocksUpdate(const Arguments& args, const VkDescriptorBufferInfo& triangleBlocks);

    void cmdUpdateVertexCount(VkCommandBuffer cmdBuffer, uint32_t vertexCount);
    void cmdUpdateMVP(VkCommandBuffer cmdBuffer, float* mvp4x4);

private:
    using BindingPosition = std::pair<size_t, size_t>;

    static constexpr BindingPosition RASTER_CONFIG_POS = {0, 0};
    static constexpr BindingPosition VERTECIES_POS = {1, 1};
    static constexpr BindingPosition COLORS_POS = {1, 2};
    static constexpr BindingPosition TRIANGLE_BLOCKS_POS = {1, 3};
    static constexpr uint32_t VERTEX_COUNT_OFFSET = 0;
    static constexpr uint32_t VERTEX_COUNT_SIZE = sizeof(uint32_t);
    static constexpr uint32_t MVP_OFFSET = VERTEX_COUNT_SIZE;
    static constexpr uint32_t MVP_SIZE = sizeof(float[4 * 4]);

    VkResult initStageCode(VkDevice dev, const VkAllocationCallbacks* allocator);
    VkResult initArgSetConfigDescription(VkDevice dev, const VkAllocationCallbacks* allocator);
    VkResult initArgSetProcessedDescription(VkDevice dev, const VkAllocationCallbacks* allocator);
    VkResult initStageDescription(VkDevice dev, const VkAllocationCallbacks* allocator);

    VkShaderModule m_stageCode;
    std::array<VkDescriptorSetLayout, ARGUMENTS_SET_COUNT> m_argSetDescriptions;
    VkPipelineLayout m_stageDescription;
};

}
}
