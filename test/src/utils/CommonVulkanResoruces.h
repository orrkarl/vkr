#pragma once

#include "ManagedVulkanResource.h"

namespace utils {

template <>
struct VulkanResourceTraits<VkPipeline> {
    static void destroy(VkDevice dev, VkPipeline& obj, const VkAllocationCallbacks* allocator) noexcept;
};

namespace factory {

ManagedVulkanResource<VkPipeline> computePipeline(VkDevice dev, const VkComputePipelineCreateInfo& info, const VkAllocationCallbacks* allocator = nullptr);

}

}
