#include "CommonVulkanResoruces.h"

#include <stdexcept>
#include <string>
#include <utility>

namespace utils {

void VulkanResourceTraits<VkPipeline>::destroy(VkDevice dev,
                                               VkPipeline& obj,
                                               const VkAllocationCallbacks* allocator) noexcept {
    vkDestroyPipeline(dev, std::exchange<VkPipeline, VkPipeline>(obj, VK_NULL_HANDLE), allocator);
}

namespace factory {

ManagedVulkanResource<VkPipeline> computePipeline(VkDevice dev,
                                                  const VkComputePipelineCreateInfo& info,
                                                  const VkAllocationCallbacks* allocator) {
    VkPipeline ret;
    VkResult status = vkCreateComputePipelines(dev, VK_NULL_HANDLE, 1, &info, allocator, &ret);
    if (status != VK_SUCCESS) {
        throw std::runtime_error("could not create pipeline: " + std::to_string(status));
    }

    return ManagedVulkanResource<VkPipeline>(dev, std::move(ret), allocator);
}

}

}
