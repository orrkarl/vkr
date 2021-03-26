namespace utils {

template <typename T>
constexpr TypedRegionDescriptor<T>::TypedRegionDescriptor() : TypedRegionDescriptor(0, 0) {
}

template <typename T>
constexpr TypedRegionDescriptor<T>::TypedRegionDescriptor(size_t offset, size_t count)
    : m_offset(offset), m_count(count) {
}

template <typename T>
constexpr TypedRegionDescriptor<T>::TypedRegionDescriptor(size_t count) : TypedRegionDescriptor(0, count) {
}

template <typename T>
constexpr size_t TypedRegionDescriptor<T>::offset() const {
    return m_offset;
}

template <typename T>
constexpr size_t TypedRegionDescriptor<T>::count() const {
    return m_count;
}

template <typename T>
constexpr size_t TypedRegionDescriptor<T>::size() const {
    return m_count * sizeof(T);
}

template <typename T>
constexpr size_t TypedRegionDescriptor<T>::end() const {
    return offset() + size();
}

template <typename T>
T* MappedMemoryGuard::hostAddress() const {
    return reinterpret_cast<T*>(m_address);
}

} // namespace utils
