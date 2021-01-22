#pragma once

#include <stdexcept>
#include <string>
#include <utility>

#include <vulkan/vulkan.h>

namespace utils {

template <typename T>
struct VulkanResourceTraits {
    static VkResult init(VkDevice dev, T& obj, const VkAllocationCallbacks* allocator);
    static void destroy(VkDevice dev, T& obj, const VkAllocationCallbacks* allocator) noexcept;
};

template <typename T>
class ManagedVulkanResource {
public:
    ManagedVulkanResource(VkDevice dev, const VkAllocationCallbacks* allocator);
    ManagedVulkanResource(VkDevice dev, T obj, const VkAllocationCallbacks* allocator);
    ~ManagedVulkanResource();

    ManagedVulkanResource(ManagedVulkanResource&& other);
    ManagedVulkanResource& operator=(ManagedVulkanResource&& other);

    ManagedVulkanResource(const ManagedVulkanResource&) = delete;
    ManagedVulkanResource& operator=(const ManagedVulkanResource&) = delete;

    T* operator->();
    const T* operator->() const;

    T& operator*();
    const T& operator*() const;

private:
    T m_object;
    VkDevice m_allocatingDevice;
    const VkAllocationCallbacks* m_allocator;
};

}

#include "ManagedVulkanResource.h"
#include "ManagedVulkanResource.inl"
