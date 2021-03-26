template <typename T>
std::vector<T> SimpleVulkanFixture::readDeviceMemory(vk::DeviceMemory memory,
                                                     const utils::TypedRegionDescriptor<T>& region) {
    std::vector<T> ret(region.count());

    utils::MappedMemoryGuard mappedRegion(context().device(), memory, region.offset(), sizeof(T) * region.count());
    std::copy(mappedRegion.hostAddress<T>(), mappedRegion.hostAddress<T>() + region.count(), ret.begin());

    return ret;
}
