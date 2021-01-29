#include "MemoryUtils.h"

#include <stdexcept>

namespace utils {

uint32_t findMemoryIndex(VkPhysicalDeviceMemoryProperties availableProperties,
                         uint32_t types,
                         VkMemoryPropertyFlags properties) {
    for (uint32_t i = 0; i < availableProperties.memoryTypeCount; ++i) {
        if ((types & (1u << i))
            && ((availableProperties.memoryTypes[i].propertyFlags & properties) == properties)) {
            return i;
        }
    }

    throw std::runtime_error("could not find appropriate memory type!");
}

} // namespace utils
