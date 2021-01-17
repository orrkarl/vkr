#pragma once

#include <vulkan/vulkan.h>

namespace vkr {

template <typename T>
class LocalManagedHandle {
public:
    using HandleType = T;

    LocalManagedHandle(T vkHandle, VkDevice& dev, const VkAllocationCallbacks*& allocator);
    ~LocalManagedHandle();

    LocalManagedHandle(const LocalManagedHandle&) = delete;
    LocalManagedHandle(LocalManagedHandle&&) = delete;

    LocalManagedHandle& operator=(const LocalManagedHandle&) = delete;
    LocalManagedHandle& operator=(LocalManagedHandle&&) = delete;

    void leak();
    T handle();

private:
    T m_handle;
    VkDevice& m_allocatingDevice;
    const VkAllocationCallbacks*& m_allocator;
};

template <typename T>
struct VkResourceTraits; /* {
    static constexpr (void*)(VkDevice, T, const VkAllocationCallbacks*) destroy = ...;
}; */

}

#include "LocalManagedHandle.inl"
