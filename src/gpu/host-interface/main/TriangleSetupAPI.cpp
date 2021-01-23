#include "TriangleSetupAPI.h"

#include "autogenerated/GPUCodeResources.h"

namespace vkr {
namespace gpu {

namespace detail {

const size_t TriangleSetupLayout::MODULE_SIZE = device_side::TriangleSetup_comp_count;
const uint32_t* TriangleSetupLayout::MODULE_DATA = device_side::TriangleSetup_comp;
const std::array<VkDescriptorSetLayoutBinding, 4> TriangleSetupLayout::BINDINGS = {
    VkDescriptorSetLayoutBinding { RASTER_CONFIG_POS.second,
                                   VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                   1,
                                   VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT,
                                   nullptr },
    VkDescriptorSetLayoutBinding { VERTECIES_POS.second,
                                   VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                   1,
                                   VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT,
                                   nullptr },
    VkDescriptorSetLayoutBinding { COLORS_POS.second,
                                   VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                   1,
                                   VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT,
                                   nullptr },
    VkDescriptorSetLayoutBinding { TRIANGLE_BLOCKS_POS.second,
                                   VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                   1,
                                   VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT,
                                   nullptr },
};
const std::array<uint32_t, 2> TriangleSetupLayout::SETS = { 1, 3 };
const std::array<VkPushConstantRange, 2> TriangleSetupLayout::PUSH_ARGS = {
    VkPushConstantRange {
        VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, VERTEX_COUNT_OFFSET, VERTEX_COUNT_SIZE },
    VkPushConstantRange { VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, MVP_OFFSET, MVP_SIZE }
};

}

VkWriteDescriptorSet
TriangleSetupAPI::describeRasterConfigUpdate(const TriangleSetupAPI::Arguments& args,
                                             const VkDescriptorBufferInfo& rasterConfigBuffer) {
    return { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             nullptr,
             args[Layout::RASTER_CONFIG_POS.first],
             Layout::RASTER_CONFIG_POS.second,
             0,
             1,
             VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
             nullptr,
             &rasterConfigBuffer,
             nullptr };
}

VkWriteDescriptorSet TriangleSetupAPI::describeVertexUpdate(const TriangleSetupAPI::Arguments& args,
                                                            const VkDescriptorBufferInfo& vertexBuffer) {
    return { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             nullptr,
             args[Layout::VERTECIES_POS.first],
             Layout::VERTECIES_POS.second,
             0,
             1,
             VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
             nullptr,
             &vertexBuffer,
             nullptr };
}

VkWriteDescriptorSet TriangleSetupAPI::describeColorUpdate(const TriangleSetupAPI::Arguments& args,
                                                           const VkDescriptorBufferInfo& colorBuffer) {
    return { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             nullptr,
             args[Layout::COLORS_POS.first],
             Layout::COLORS_POS.second,
             0,
             1,
             VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
             nullptr,
             &colorBuffer,
             nullptr };
}

VkWriteDescriptorSet
TriangleSetupAPI::describeTriangleBlocksUpdate(const TriangleSetupAPI::Arguments& args,
                                               const VkDescriptorBufferInfo& triangleBlocks) {
    return { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             nullptr,
             args[Layout::TRIANGLE_BLOCKS_POS.first],
             Layout::TRIANGLE_BLOCKS_POS.second,
             0,
             1,
             VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
             nullptr,
             &triangleBlocks,
             nullptr };
}

void TriangleSetupAPI::cmdUpdateVertexCount(VkCommandBuffer cmdBuffer, uint32_t vertexCount) {
    vkCmdPushConstants(cmdBuffer,
                       m_runnerDesc,
                       VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT,
                       Layout::VERTEX_COUNT_OFFSET,
                       Layout::VERTEX_COUNT_SIZE,
                       &vertexCount);
}

void TriangleSetupAPI::cmdUpdateMVP(VkCommandBuffer cmdBuffer, float* mvp4x4) {
    vkCmdPushConstants(cmdBuffer,
                       m_runnerDesc,
                       VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT,
                       Layout::MVP_OFFSET,
                       Layout::MVP_SIZE,
                       mvp4x4);
}

}
}
