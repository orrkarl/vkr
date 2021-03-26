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

template <typename T>
struct TypedRegionDescriptor {
    using Type = T;

    constexpr TypedRegionDescriptor();
    explicit constexpr TypedRegionDescriptor(size_t offset, size_t count);
    explicit constexpr TypedRegionDescriptor(size_t count);

    constexpr size_t offset() const;
    constexpr size_t count() const;
    constexpr size_t size() const;
    constexpr size_t end() const;

private:
    size_t m_offset;
    size_t m_count;
};

class MappedMemoryGuard {
public:
    MappedMemoryGuard(vk::Device dev, vk::DeviceMemory memory, vk::DeviceSize regionOffset, vk::DeviceSize regionSize);
    ~MappedMemoryGuard();

    template <typename T>
    T* hostAddress() const;

private:
    MappedMemoryGuard(const MappedMemoryGuard&) = delete;
    MappedMemoryGuard& operator=(const MappedMemoryGuard&) = delete;
    MappedMemoryGuard(MappedMemoryGuard&&) = delete;
    MappedMemoryGuard& operator=(MappedMemoryGuard&&) = delete;

    vk::Device m_device;
    vk::DeviceMemory m_memory;
    void* m_address;
};

} // namespace utils

#include "MemoryUtils.inl"
