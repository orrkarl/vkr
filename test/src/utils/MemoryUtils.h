#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

namespace utils {

uint32_t findMemoryIndex(VkPhysicalDeviceMemoryProperties availableProperties,
                         uint32_t types,
                         VkMemoryPropertyFlags properties);

vk::MemoryRequirements describeBatchAllocation(const std::vector<vk::MemoryRequirements>& requirements);

void bindBuffers(vk::Device dev,
                 vk::DeviceMemory memory,
                 const std::vector<vk::Buffer>& buffers,
                 const std::vector<vk::MemoryRequirements>& requirements);

} // namespace utils
