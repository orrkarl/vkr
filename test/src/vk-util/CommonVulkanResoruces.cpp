#include "CommonVulkanResoruces.h"

#include <utility>

namespace utils {

void VulkanResourceTraits<VkPipeline>::destroy(VkDevice dev,
                                               VkPipeline& obj,
                                               const VkAllocationCallbacks* allocator) noexcept {
    vkDestroyPipeline(dev, std::exchange<VkPipeline, VkPipeline>(obj, VK_NULL_HANDLE), allocator);
}

}
