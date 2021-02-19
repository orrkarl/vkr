#pragma once

#include <VkBootstrap.h>

#include <vulkan/vulkan.hpp>

#include "IDebugMessenger.h"

namespace utils {

class VulkanContext {
public:
    VulkanContext(std::vector<std::reference_wrapper<IDebugMessenger>> instanceMessengers);

    VulkanContext(const VulkanContext&) = delete;
    VulkanContext& operator=(const VulkanContext&) = delete;

    VulkanContext(VulkanContext&& other) = delete;
    VulkanContext& operator=(VulkanContext&& other) = delete;

    ~VulkanContext();

    vk::Instance instance() const;
    const vkb::PhysicalDevice& physicalDevice() const;
    vk::Device device() const;

    vk::Queue computeQueue() const;

    vk::UniqueDeviceMemory allocate(vk::MemoryRequirements allocation,
                                    vk::MemoryPropertyFlags properties,
                                    vk::Optional<const vk::AllocationCallbacks> allocator = nullptr);

    vk::UniqueDeviceMemory satisfyBuffersMemory(const std::vector<vk::Buffer>& buffers,
                                                vk::MemoryPropertyFlags properties,
                                                vk::Optional<const vk::AllocationCallbacks> allocator = nullptr);

    vk::UniqueCommandPool createComputeCommnadPool();

    const vk::DispatchLoaderDynamic& dynamics() const;

private:
    vkb::Instance m_instance;
    vk::DispatchLoaderDynamic m_dynamics;
    vkb::PhysicalDevice m_physicalDevice;
    vkb::Device m_device;
    vk::Queue m_computeQueue;
    uint32_t m_computeFamilyIndex;
};

} // namespace utils
