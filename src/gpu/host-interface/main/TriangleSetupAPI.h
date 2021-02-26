#pragma once

#include <array>
#include <utility>

#include <vulkan/vulkan.h>

#include "../DeviceModuleAPI.h"

namespace vkr {
namespace gpu {

namespace detail {

struct TriangleSetupLayout {
    static const size_t MODULE_SIZE;
    static const uint32_t* MODULE_DATA;
    static const std::array<VkDescriptorSetLayoutBinding, 4> BINDINGS;
    static const std::array<uint32_t, 2> SETS;
    static const VkPushConstantRange PUSH_ARGS;
    static const std::array<uint32_t, 3> GROUP_SIZES;

    static constexpr std::pair<uint32_t, uint32_t> RASTER_CONFIG_POS = { 0, 0 };
    static constexpr std::pair<uint32_t, uint32_t> VERTECIES_POS = { 1, 1 };
    static constexpr std::pair<uint32_t, uint32_t> COLORS_POS = { 1, 2 };
    static constexpr std::pair<uint32_t, uint32_t> TRIANGLE_BLOCKS_POS = { 1, 3 };

    static constexpr uint32_t MVP_OFFSET = 0;
    static constexpr uint32_t MVP_SIZE = sizeof(float[4 * 4]);
    static constexpr uint32_t VERTEX_COUNT_OFFSET = MVP_SIZE;
    static constexpr uint32_t VERTEX_COUNT_SIZE = sizeof(uint32_t);
};

} // namespace detail

class TriangleSetupAPI : public DeviceModuleAPI<detail::TriangleSetupLayout> {
public:
    using Arguments = std::array<VkDescriptorSet, Layout::BINDINGS.size()>;

    VkWriteDescriptorSet describeRasterConfigUpdate(const Arguments& args,
                                                    const VkDescriptorBufferInfo& rasterConfigBuffer);
    VkWriteDescriptorSet describeVertexUpdate(const Arguments& args, const VkDescriptorBufferInfo& vertexBuffer);
    VkWriteDescriptorSet describeColorUpdate(const Arguments& args, const VkDescriptorBufferInfo& colorBuffer);
    VkWriteDescriptorSet describeTriangleBlocksUpdate(const Arguments& args,
                                                      const VkDescriptorBufferInfo& triangleBlocks);

    void cmdUpdateVertexCount(VkCommandBuffer cmdBuffer, uint32_t vertexCount);
    void cmdUpdateMVP(VkCommandBuffer cmdBuffer, float* mvp4x4);
};

} // namespace gpu
} // namespace vkr
