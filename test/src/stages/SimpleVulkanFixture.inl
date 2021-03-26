template <typename T>
std::vector<T> SimpleVulkanFixture::readDeviceMemory(vk::DeviceMemory memory, size_t offset, size_t count) {
    std::vector<T> ret(count);

    utils::MappedMemoryGuard mappedRegion(context().device(), memory, offset, sizeof(T) * count);
    std::copy(mappedRegion.hostAddress<T>(), mappedRegion.hostAddress<T>() + count, ret.begin());

    return ret;
}
