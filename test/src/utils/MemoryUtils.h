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

class MappedMemoryGuard {
public:
    MappedMemoryGuard(vk::Device& dev,
                      vk::DeviceMemory& memory,
                      vk::DeviceSize regionSize,
                      vk::DeviceSize regionOffset = 0);
    ~MappedMemoryGuard();

    template <typename T>
    T* hostAddress() const;

private:
    MappedMemoryGuard(const MappedMemoryGuard&) = delete;
    MappedMemoryGuard& operator=(const MappedMemoryGuard&) = delete;
    MappedMemoryGuard(MappedMemoryGuard&&) = delete;
    MappedMemoryGuard& operator=(MappedMemoryGuard&&) = delete;

    vk::Device& m_device;
    vk::DeviceMemory& m_memory;
    void* m_address;
};

} // namespace utils

#include "MemoryUtils.inl"
