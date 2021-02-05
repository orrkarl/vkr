namespace utils {

template <typename T>
T* MappedMemoryGuard::hostAddress() const {
    return reinterpret_cast<T*>(m_address);
}

} // namespace utils
