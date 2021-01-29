#pragma once

#include "ManagedVulkanResource.h"

namespace utils {

template <>
struct VulkanResourceTraits<VkPipeline> {
    static void destroy(VkDevice dev, VkPipeline& obj, const VkAllocationCallbacks* allocator) noexcept;
};

}
