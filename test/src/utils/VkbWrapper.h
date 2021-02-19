#pragma once

#include <VkBootstrap.h>

#include <vulkan/vulkan.hpp>

namespace utils {

template <typename T>
struct VkbObjectTraits {};

template <>
struct VkbObjectTraits<vkb::Instance> {
    static constexpr auto deleter = vkb::destroy_instance;
};

template <>
struct VkbObjectTraits<vkb::Device> {
    static constexpr auto deleter = vkb::destroy_device;
};

template <typename T>
class VkbWrapper {
public:
    using Traits = VkbObjectTraits<T>;

    VkbWrapper() : m_obj(T {}) {
    }
    VkbWrapper(T obj) : m_obj(obj) {
    }
    ~VkbWrapper() {
        Traits::deleter(m_obj);
    }

    operator T&() {
        return m_obj;
    }
    T& get() {
        return m_obj;
    }
    T* operator->() {
        return &m_obj;
    }
    operator const T&() const {
        return m_obj;
    }
    const T& get() const {
        return m_obj;
    }
    const T* operator->() const {
        return &m_obj;
    }

private:
    T m_obj;
};

} // namespace utils
