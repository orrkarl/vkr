#include "CommonVulkanResoruces.h"

#include <string>
#include <utility>

#include "VulkanError.h"

namespace utils {

void VulkanResourceTraits<VkPipeline>::destroy(VkDevice dev,
                                               VkPipeline& obj,
                                               const VkAllocationCallbacks* allocator) noexcept {
    vkDestroyPipeline(dev, std::exchange(obj, nullptr), allocator);
}

void VulkanResourceTraits<VkDeviceMemory>::destroy(VkDevice dev,
                                                   VkDeviceMemory& mem,
                                                   const VkAllocationCallbacks* allocator) noexcept {
    vkFreeMemory(dev, std::exchange(mem, nullptr), allocator);
}

void VulkanResourceTraits<VkBuffer>::destroy(VkDevice dev,
                                             VkBuffer& mem,
                                             const VkAllocationCallbacks* allocator) noexcept {
    vkDestroyBuffer(dev, std::exchange(mem, nullptr), allocator);
}

namespace factory {

ManagedVulkanResource<VkPipeline> computePipeline(VkDevice dev,
                                                  const VkComputePipelineCreateInfo& info,
                                                  const VkAllocationCallbacks* allocator) {
    VkPipeline ret;
    VkResult status = vkCreateComputePipelines(dev, nullptr, 1, &info, allocator, &ret);
    if (status != VK_SUCCESS) {
        throw utils::VulkanError("could not create pipeline", status);
    }

    return ManagedVulkanResource(dev, ret, allocator);
}

ManagedVulkanResource<VkBuffer> buffer(VkDevice dev,
                                       const VkBufferCreateInfo& info,
                                       const VkAllocationCallbacks* allocator) {
    VkBuffer ret;
    auto status = vkCreateBuffer(dev, &info, allocator, &ret);
    if (status != VK_SUCCESS) {
        throw utils::VulkanError("could not create buffer", status);
    }

    return ManagedVulkanResource(dev, ret, allocator);
}

} // namespace factory

} // namespace utils
