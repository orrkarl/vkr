#pragma once

#include <VkBootstrap.h>

#include <vulkan/vulkan.hpp>

#include "CommonDebugMessengers.h"
#include "VkbWrapper.h"

namespace utils {

class VulkanContext {
public:
    VulkanContext(std::unique_ptr<ChainedDebugMessenger> messenger);

    VulkanContext(const VulkanContext&) = delete;
    VulkanContext& operator=(const VulkanContext&) = delete;

    VulkanContext(VulkanContext&& other) = delete;
    VulkanContext& operator=(VulkanContext&& other) = delete;

    ~VulkanContext() = default;

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
    std::unique_ptr<ChainedDebugMessenger> m_messenger;
    VkbWrapper<vkb::Instance> m_instance;
    vk::DispatchLoaderDynamic m_dynamics;
    std::unique_ptr<DebugMessengerRegisterGuard> m_messengerRegistration;
    vkb::PhysicalDevice m_physicalDevice;
    VkbWrapper<vkb::Device> m_device;
    vk::Queue m_computeQueue;
    uint32_t m_computeFamilyIndex;
};

} // namespace utils
