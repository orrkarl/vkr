#include "VulkanContext.h"

#include <algorithm>
#include <iostream>

#include <catch2/catch_test_macros.hpp>
#include <vulkan/vulkan.hpp>

#include "MemoryUtils.h"

namespace vkb::detail {
VkQueue get_queue(VkDevice device, uint32_t family);
}

namespace utils {

std::vector<VkDebugUtilsMessengerCreateInfoEXT> extractMessengers(
    std::vector<std::unique_ptr<IDebugMessenger>>& messengers) {
    std::vector<VkDebugUtilsMessengerCreateInfoEXT> ret(messengers.size());
    std::transform(messengers.begin(), messengers.end(), ret.begin(), [](std::unique_ptr<IDebugMessenger>& messenger) {
        return messenger->describeMessenger();
    });
    return ret;
}

VulkanContext::VulkanContext() {
    m_validationFailures = 0;
    m_debugMessengers.emplace_back(
        std::make_unique<OstreamLoggingMessenger>(std::cout, vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning));
    m_debugMessengers.emplace_back(std::make_unique<SeverityCountMessenger>(
        m_validationFailures,
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError));

    vkb::InstanceBuilder instBuild;
    auto inst = instBuild.set_app_name("vkr-test-suite")
                    .request_validation_layers()
                    .enable_extension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
                    .require_api_version(1, 1, 0)
                    .set_headless()
                    .provide_instance_debug_messengers(extractMessengers(m_debugMessengers))
                    .build();
    if (!inst) {
        throw std::runtime_error("could not create instance: " + inst.error().message());
    }
    m_instance = inst.value();
    m_dynamics = vk::DispatchLoaderDynamic(m_instance->instance, vkGetInstanceProcAddr);

    for (auto& messenger : m_debugMessengers) {
        m_messengersRegistration.emplace_back(m_instance->instance, m_dynamics, *messenger);
    }

    vkb::PhysicalDeviceSelector pdevSelect(m_instance);
    VkPhysicalDeviceFeatures requiredFeatures {};
    requiredFeatures.shaderInt16 = VK_TRUE;
    requiredFeatures.shaderInt64 = VK_TRUE;
    auto pdev = pdevSelect.set_minimum_version(1, 1)
                    .prefer_gpu_device_type(vkb::PreferredDeviceType::discrete)
                    .set_required_features(requiredFeatures)
                    .select();
    if (!pdev) {
        throw std::runtime_error("could not pick physical device: " + pdev.error().message());
    }
    m_physicalDevice = pdev.value();

    vkb::DeviceBuilder devBuild(m_physicalDevice);
    auto dev = devBuild.build();
    if (!dev) {
        throw std::runtime_error("could not build logical device: " + dev.error().message());
    }
    m_device = dev.value();

    auto computeQueueFamily = std::find_if(
        m_device->queue_families.cbegin(), m_device->queue_families.cend(), [](const auto& family) {
            return (family.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0;
        });
    if (computeQueueFamily == m_device->queue_families.cend()) {
        throw std::runtime_error("could not acquire compute queue: "
                                 + vkb::make_error_code(vkb::QueueError::compute_unavailable).message());
    }

    m_computeFamilyIndex = static_cast<uint32_t>(computeQueueFamily - m_device->queue_families.cbegin());
    m_computeQueue = vk::Queue(vkb::detail::get_queue(m_device->device, m_computeFamilyIndex));
}

VulkanContext::~VulkanContext() {
}

vk::Instance VulkanContext::instance() const {
    return m_instance->instance;
}

const vkb::PhysicalDevice& VulkanContext::physicalDevice() const {
    return m_physicalDevice;
}

vk::Device VulkanContext::device() const {
    return vk::Device(m_device->device);
}

vk::Queue VulkanContext::computeQueue() const {
    return m_computeQueue;
}

vk::UniqueDeviceMemory VulkanContext::allocate(vk::MemoryRequirements allocation,
                                               vk::MemoryPropertyFlags properties,
                                               vk::Optional<const vk::AllocationCallbacks> allocator) {
    vk::PhysicalDeviceMemoryProperties props(m_physicalDevice.memory_properties);

    VkMemoryAllocateInfo alloc { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                 nullptr,
                                 allocation.size,
                                 findMemoryIndex(props, allocation.memoryTypeBits, properties) };

    return device().allocateMemoryUnique(alloc, allocator);
}

vk::UniqueDeviceMemory VulkanContext::satisfyBuffersMemory(const std::vector<vk::Buffer>& buffers,
                                                           vk::MemoryPropertyFlags properties,
                                                           vk::Optional<const vk::AllocationCallbacks> allocator) {

    std::vector<vk::MemoryRequirements> requirements(buffers.size());
    std::transform(buffers.cbegin(), buffers.cend(), requirements.begin(), [this](auto buffer) {
        return this->device().getBufferMemoryRequirements(buffer);
    });

    auto memory = allocate(describeBatchAllocation(requirements), properties, allocator);
    bindBuffers(device(), *memory, buffers, requirements);

    return memory;
}

vk::UniqueCommandPool VulkanContext::createComputeCommnadPool() {
    return device().createCommandPoolUnique(
        vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlags(), m_computeFamilyIndex));
}

const vk::DispatchLoaderDynamic& VulkanContext::dynamics() const {
    return m_dynamics;
}

} // namespace utils
