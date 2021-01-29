#include "CommonVulkanResoruces.h"

#include <stdexcept>
#include <string>
#include <utility>

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

namespace factory {

ManagedVulkanResource<VkPipeline> computePipeline(VkDevice dev,
                                                  const VkComputePipelineCreateInfo& info,
                                                  const VkAllocationCallbacks* allocator) {
    VkPipeline ret;
    VkResult status = vkCreateComputePipelines(dev, nullptr, 1, &info, allocator, &ret);
    if (status != VK_SUCCESS) {
        throw std::runtime_error("could not create pipeline: " + std::to_string(status));
    }

    return ManagedVulkanResource<VkPipeline>(dev, std::move(ret), allocator);
}

} // namespace factory

} // namespace utils
