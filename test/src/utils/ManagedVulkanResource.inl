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
ManagedVulkanResource<T>::ManagedVulkanResource(vk::Device dev, vk::Optional<const vk::AllocationCallbacks> allocator)
    : m_allocatingDevice(dev), m_allocator(allocator), m_object() {
    vk::Result status = VulkanResourceTraits<T>::init(dev, m_object, allocator);
    if (status != vk::Result::eSuccess) {
        vk::throwResultException(status, "call to init() failed");
    }
}

template <typename T>
ManagedVulkanResource<T>::ManagedVulkanResource(vk::Device dev,
                                                T obj,
                                                vk::Optional<const vk::AllocationCallbacks> allocator)
    : m_allocatingDevice(dev), m_allocator(allocator), m_object(std::move(obj)) {
}

template <typename T>
ManagedVulkanResource<T>::~ManagedVulkanResource() {
    reset();
}

template <typename T>
ManagedVulkanResource<T>::ManagedVulkanResource(ManagedVulkanResource&& other)
    : m_allocatingDevice(std::exchange(other.m_allocatingDevice, vk::Device()))
    , m_allocator(std::exchange(other.m_allocator, nullptr))
    , m_object(std::exchange(other.m_object, T())) {
}

template <typename T>
ManagedVulkanResource<T>& ManagedVulkanResource<T>::operator=(ManagedVulkanResource&& other) {
    reset();
    m_allocatingDevice = std::exchange(other.m_allocatingDevice, vk::Device());
    m_allocator = std::exchange(other.m_allocator, nullptr);
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

template <typename T>
void utils::ManagedVulkanResource<T>::reset() {
    if (m_allocatingDevice != vk::Device()) {
        VulkanResourceTraits<T>::destroy(m_allocatingDevice, m_object, m_allocator);
        m_object = T();
        m_allocator = nullptr;
        m_allocatingDevice = vk::Device();
    }
}

} // namespace utils
