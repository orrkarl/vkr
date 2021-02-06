#pragma once

#include <array>

#include <vulkan/vulkan.h>

#include "../../../shared/compatibility.h"
#include "../../DeviceModuleAPI.h"

namespace vkr {
namespace gpu {
namespace tests {

namespace detail {

struct ClippingLayout {
    static const size_t MODULE_SIZE;
    static const uint32_t* MODULE_DATA;
    static const std::array<VkDescriptorSetLayoutBinding, 3> BINDINGS;
    static const std::array<uint32_t, 1> SETS;
    static const std::array<VkPushConstantRange, 1> PUSH_ARGS;
    static const std::array<uint32_t, 3> GROUP_SIZES;

    static constexpr std::pair<uint32_t, uint32_t> VERTECIES_POS = { 0, 0 };
    static constexpr std::pair<uint32_t, uint32_t> CLIPPED_VERTECIES_POS = { 0, 1 };
    static constexpr std::pair<uint32_t, uint32_t> CLIPPED_VERTEX_COUNTS = { 0, 2 };

    static constexpr uint32_t TRIANGLE_COUNT_OFFSET = 0;
    static constexpr uint32_t TRIANGLE_COUNT_SIZE = sizeof(uint32_t);
};

} // namespace detail

class ClippingAPI : public DeviceModuleAPI<detail::ClippingLayout> {
public:
    VkWriteDescriptorSet describeVerteciesUpdate(VkDescriptorSet args,
                                                 const VkDescriptorBufferInfo& vertecies);
    VkWriteDescriptorSet describeClippedVerteciesUpdate(VkDescriptorSet args,
                                                        const VkDescriptorBufferInfo& clippedVertecies);
    VkWriteDescriptorSet describeClippedVertexCountsUpdate(VkDescriptorSet args,
                                                           const VkDescriptorBufferInfo& clippedVertexCounts);

    void cmdUpdateTriangleCount(VkCommandBuffer cmdBuffer, u32 triangleCount);
};

} // namespace tests
} // namespace gpu
} // namespace vkr
