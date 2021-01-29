namespace utils {

template <typename T>
vk::Result VulkanResourceTraits<T>::init(vk::Device dev,
                                       T& obj,
                                       vk::Optional<const vk::AllocationCallbacks> allocator) {
    auto actualAlloc = static_cast<const vk::AllocationCallbacks*>(allocator);
    return static_cast<vk::Result>(obj.init(dev, reinterpret_cast<const VkAllocationCallbacks*>(actualAlloc)));
}

template <typename T>
void VulkanResourceTraits<T>::destroy(vk::Device dev,
                                      T& obj,
                                      vk::Optional<const vk::AllocationCallbacks> allocator) noexcept {
    auto actualAlloc = static_cast<const vk::AllocationCallbacks*>(allocator);
    obj.destroy(dev, reinterpret_cast<const VkAllocationCallbacks*>(actualAlloc));
}

template <typename T>
ManagedVulkanResource<T>::ManagedVulkanResource(vk::Device dev,
                                                vk::Optional<const vk::AllocationCallbacks> allocator)
    : m_object(), m_allocatingDevice(dev), m_allocator(allocator) {
    vk::Result status = VulkanResourceTraits<T>::init(dev, m_object, allocator);
    if (status != vk::Result::eSuccess) {
        vk::throwResultException(status, "call to init() failed");
    }
}

template <typename T>
ManagedVulkanResource<T>::ManagedVulkanResource(vk::Device dev,
                                                T obj,
                                                vk::Optional<const vk::AllocationCallbacks> allocator)
    : m_object(std::move(obj)), m_allocatingDevice(dev), m_allocator(allocator) {
}

template <typename T>
ManagedVulkanResource<T>::~ManagedVulkanResource() {
    VulkanResourceTraits<T>::destroy(m_allocatingDevice, m_object, m_allocator);
}

template <typename T>
ManagedVulkanResource<T>::ManagedVulkanResource(ManagedVulkanResource&& other) {
    m_object = std::exchange(other.m_object, T());
}

template <typename T>
ManagedVulkanResource<T>& ManagedVulkanResource<T>::operator=(ManagedVulkanResource&& other) {
    VulkanResourceTraits<T>::destroy(m_allocatingDevice, m_object, m_allocator);
    m_object = std::exchange(other.m_object, T());
    return *this;
}

template <typename T>
T* ManagedVulkanResource<T>::operator->() {
    return &m_object;
}

template <typename T>
const T* ManagedVulkanResource<T>::operator->() const {
    return &m_object;
}

template <typename T>
const T& ManagedVulkanResource<T>::operator*() const {
    return m_object;
}

template <typename T>
T& ManagedVulkanResource<T>::operator*() {
    return m_object;
}

} // namespace utils
