#pragma once

namespace vkr {

template<typename T>
LocalManagedHandle<T>::LocalManagedHandle(T vkHandle, VkDevice &dev, const VkAllocationCallbacks*& allocator)
    : m_handle(vkHandle), m_allocatingDevice(dev), m_allocator(allocator) {
}

template<typename T>
LocalManagedHandle<T>::~LocalManagedHandle() {
    if (m_handle != VK_NULL_HANDLE) {
        VkResourceTraits<T>::destroy(m_allocatingDevice, m_handle, m_allocator);
    }
}

template<typename T>
void LocalManagedHandle<T>::leak() {
    m_handle = VK_NULL_HANDLE;
}

template<typename T>
T LocalManagedHandle<T>::handle() {
    return m_handle;
}

}
