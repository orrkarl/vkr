#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

namespace utils {

uint32_t findMemoryIndex(vk::PhysicalDeviceMemoryProperties availableProperties,
                         uint32_t types,
                         vk::MemoryPropertyFlags properties);

vk::MemoryRequirements describeBatchAllocation(const std::vector<vk::MemoryRequirements>& requirements);

void bindBuffers(vk::Device dev,
                 vk::DeviceMemory memory,
                 const std::vector<vk::Buffer>& buffers,
                 const std::vector<vk::MemoryRequirements>& requirements);

} // namespace utils
