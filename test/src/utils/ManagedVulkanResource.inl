namespace utils {

template<typename T>
VkResult VulkanResourceTraits<T>::init(VkDevice dev, T& obj, const VkAllocationCallbacks *allocator) {
    return obj.init(dev, allocator);
}

template<typename T>
void VulkanResourceTraits<T>::destroy(VkDevice dev, T& obj, const VkAllocationCallbacks *allocator) noexcept {
    obj.destroy(dev, allocator);
}

template<typename T>
ManagedVulkanResource<T>::ManagedVulkanResource(VkDevice dev, const VkAllocationCallbacks *allocator)
    : m_object()
    , m_allocatingDevice(dev)
    , m_allocator(allocator) {
    VkResult status = VulkanResourceTraits<T>::init(dev, m_object, allocator);
    if (status != VK_SUCCESS) {
        throw std::runtime_error("got vulkan error: " + std::to_string(status));
    }
}

template<typename T>
ManagedVulkanResource<T>::ManagedVulkanResource(VkDevice dev, T obj, const VkAllocationCallbacks *allocator)
    : m_object(std::move(obj))
    , m_allocatingDevice(dev)
    , m_allocator(allocator) {
}

template<typename T>
ManagedVulkanResource<T>::~ManagedVulkanResource() {
    VulkanResourceTraits<T>::destroy(m_allocatingDevice, m_object, m_allocator);
}

template<typename T>
ManagedVulkanResource<T>::ManagedVulkanResource(ManagedVulkanResource&& other) {
    m_object = std::exchange(other.m_object, T());
}

template<typename T>
ManagedVulkanResource<T>& ManagedVulkanResource<T>::operator=(ManagedVulkanResource&& other) {
    VulkanResourceTraits<T>::destroy(m_allocatingDevice, m_object, m_allocator);
    m_object = std::exchange(other.m_object, T());
    return *this;
}

template<typename T>
T* ManagedVulkanResource<T>::operator->() {
    return &m_object;
}

template<typename T>
const T* ManagedVulkanResource<T>::operator->() const {
    return &m_object;
}

template<typename T>
const T& ManagedVulkanResource<T>::operator*() const {
    return m_object;
}

template<typename T>
T& ManagedVulkanResource<T>::operator*() {
    return m_object;
}

}
