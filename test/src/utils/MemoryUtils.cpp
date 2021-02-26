#include "MemoryUtils.h"

#include <algorithm>
#include <numeric>
#include <stdexcept>

namespace utils {

template <typename T>
T align(T value, T alignment) {
    auto alignBits = alignment - 1;
    return (value + alignBits) & (~alignBits);
}

uint32_t findMemoryIndex(vk::PhysicalDeviceMemoryProperties availableProperties,
                         uint32_t types,
                         vk::MemoryPropertyFlags properties) {
    for (uint32_t i = 0; i < availableProperties.memoryTypeCount; ++i) {
        if ((types & (1u << i)) && ((availableProperties.memoryTypes[i].propertyFlags & properties) == properties)) {
            return i;
        }
    }

    throw std::runtime_error("could not find appropriate memory type!");
}

vk::MemoryRequirements describeBatchAllocation(const std::vector<vk::MemoryRequirements>& requirements) {
    // total size is the size of all needed sizes, each placed in it's required alignment
    vk::DeviceSize size = std::accumulate(requirements.cbegin(),
                                          requirements.cend(),
                                          vk::DeviceSize(0),
                                          [](vk::DeviceSize collectedSize, vk::MemoryRequirements curr) {
                                              return align(collectedSize, curr.alignment) + curr.size;
                                          });

    // allignment is just the maximum allignment of all elements
    vk::DeviceSize alignment = std::max_element(requirements.cbegin(), requirements.cend(), [](auto req1, auto req2) {
                                   return req1.alignment < req2.alignment;
                               })->alignment;

    // flags are the intersection of all type flags
    uint32_t flags = std::accumulate(
        requirements.cbegin(),
        requirements.cend(),
        uint32_t(-1),
        [](uint32_t collectedFlags, vk::MemoryRequirements curr) { return collectedFlags & curr.memoryTypeBits; });

    return { size, alignment, flags };
}

void bindBuffers(vk::Device dev,
                 vk::DeviceMemory memory,
                 const std::vector<vk::Buffer>& buffers,
                 const std::vector<vk::MemoryRequirements>& requirements) {
    if (buffers.size() != requirements.size()) {
        throw std::runtime_error("buffers cound different than requirements count");
    }

    vk::DeviceSize offset = 0;
    for (size_t i = 0; i < buffers.size(); ++i) {
        offset = align(offset, requirements[i].alignment);
        dev.bindBufferMemory(buffers[i], memory, offset);
        offset += requirements[i].size;
    }
}

MappedMemoryGuard::MappedMemoryGuard(vk::Device& dev,
                                     vk::DeviceMemory& memory,
                                     vk::DeviceSize regionOffset,
                                     vk::DeviceSize regionSize)
    : m_device(dev), m_memory(memory), m_address(nullptr) {
    m_address = dev.mapMemory(memory, regionOffset, regionSize);
}

MappedMemoryGuard::~MappedMemoryGuard() {
    m_device.unmapMemory(m_memory);
}

} // namespace utils
