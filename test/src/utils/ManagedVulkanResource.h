#pragma once

#include <stdexcept>
#include <string>
#include <utility>

#include <vulkan/vulkan.hpp>

namespace utils {

template <typename T>
struct VulkanResourceTraits {
    static vk::Result init(vk::Device dev, T& obj, vk::Optional<const vk::AllocationCallbacks> allocator);
    static void destroy(vk::Device dev, T& obj, vk::Optional<const vk::AllocationCallbacks> allocator) noexcept;
};

template <typename T>
class ManagedVulkanResource {
public:
    ManagedVulkanResource() : m_allocatingDevice(), m_allocator(nullptr), m_object() {
    }
    ManagedVulkanResource(vk::Device dev, vk::Optional<const vk::AllocationCallbacks> allocator);
    ManagedVulkanResource(vk::Device dev, T obj, vk::Optional<const vk::AllocationCallbacks> allocator);
    ~ManagedVulkanResource();

    ManagedVulkanResource(ManagedVulkanResource&& other);
    ManagedVulkanResource& operator=(ManagedVulkanResource&& other);

    void reset();

    ManagedVulkanResource(const ManagedVulkanResource&) = delete;
    ManagedVulkanResource& operator=(const ManagedVulkanResource&) = delete;

    T* operator->();
    const T* operator->() const;

    T& operator*();
    const T& operator*() const;

private:
    vk::Device m_allocatingDevice;
    vk::Optional<const vk::AllocationCallbacks> m_allocator;
    T m_object;
};

} // namespace utils

#include "ManagedVulkanResource.inl"
