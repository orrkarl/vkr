#pragma once

#include <vulkan/vulkan.h>

#include "../../../shared/compatibility.h"

namespace vkr {
namespace tests {
namespace gpu {

class ClippingAPI {
public:
    explicit ClippingAPI();
    VkResult init(VkDevice dev, const VkAllocationCallbacks* allocator);
    void destroy(VkDevice dev, const VkAllocationCallbacks* allocator);

    VkDescriptorSetLayout describeArguments();
    VkComputePipelineCreateInfo describeRunner();

    VkWriteDescriptorSet describeVerteciesUpdate(VkDescriptorSet args, const VkDescriptorBufferInfo& rasterConfigBuffer);
    VkWriteDescriptorSet describeClippedVerteciesUpdate(VkDescriptorSet args, const VkDescriptorBufferInfo& vertexBuffer);
    VkWriteDescriptorSet describeClippedVertexCountsUpdate(VkDescriptorSet args, const VkDescriptorBufferInfo& colorBuffer);

    void cmdUpdateTriangleCount(VkCommandBuffer cmdBuffer, u32 triangleCount);

private:
    static constexpr uint32_t VERTECIES_POS = 0;
    static constexpr uint32_t CLIPPED_VERTECIES_POS = 1;
    static constexpr uint32_t CLIPPED_VERTEX_COUNTS_POS = 2;

    VkResult initClippingCode(VkDevice dev, const VkAllocationCallbacks* allocator);
    VkResult initArgsDescription(VkDevice dev, const VkAllocationCallbacks* allocator);
    VkResult initClippingDescription(VkDevice dev, const VkAllocationCallbacks* allocator);

    VkShaderModule m_clippingCode;
    VkDescriptorSetLayout m_argsDesc;
    VkPipelineLayout m_clippingRunnerDesc;
};

}
}
}
