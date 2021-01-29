#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace utils {

uint32_t findMemoryIndex(VkPhysicalDeviceMemoryProperties availableProperties, uint32_t types, VkMemoryPropertyFlags properties);

}
