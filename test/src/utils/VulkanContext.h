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
    const vkb::Device& device() const;
    VkDevice rawDevice();

    VkQueue computeQueue() const;

    VkDeviceMemory allocate(VkMemoryRequirements allocation,
                            VkMemoryPropertyFlags properties,
                            const VkAllocationCallbacks* allocator = nullptr);

private:
    uint32_t findMemoryIndex(uint32_t types, VkMemoryPropertyFlags properties) const;

    vkb::Instance m_instance;
    vkb::PhysicalDevice m_physicalDevice;
    vkb::Device m_device;
    VkQueue m_computeQueue;
};

} // namespace utils
