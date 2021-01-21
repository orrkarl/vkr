#pragma once

#include <VkBootstrap.h>

namespace utils {

class VulkanContext {
public:
    VulkanContext();

    VulkanContext(const VulkanContext&) = delete;
    VulkanContext& operator=(const VulkanContext&) = delete;

    VulkanContext(VulkanContext&& other) = delete;
    VulkanContext& operator=(VulkanContext&& other) = delete;

    ~VulkanContext();

    const vkb::Instance& instance() const;
    const vkb::PhysicalDevice& physicalDevice() const;
    const vkb::Device& device();

    VkQueue compute() const;

private:
    vkb::Instance m_instance;
    vkb::PhysicalDevice m_physicalDevice;
    vkb::Device m_device;
    VkQueue m_computeQueue;
};

} // namespace vk_util

