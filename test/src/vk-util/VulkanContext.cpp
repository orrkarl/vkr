#include "VulkanContext.h"

#include <algorithm>

namespace vkb::detail {
VkQueue get_queue(VkDevice device, uint32_t family);
}

namespace utils {

VulkanContext::VulkanContext() {
    vkb::InstanceBuilder instBuild;
    auto inst = instBuild.set_app_name("vkr-test-suite")
                    .request_validation_layers()
                    .require_api_version(1, 1, 0)
                    .use_default_debug_messenger()
                    .set_headless()
                    .build();
    if (!inst) {
        throw std::runtime_error("could not create instance: " + inst.error().message());
    }
    m_instance = inst.value();

    vkb::PhysicalDeviceSelector pdevSelect(m_instance);
    VkPhysicalDeviceFeatures requiredFeatures {};
    requiredFeatures.shaderInt16 = VK_TRUE;
    requiredFeatures.shaderInt64 = VK_TRUE;
    auto pdev = pdevSelect.set_minimum_version(1, 1)
                    .prefer_gpu_device_type(vkb::PreferredDeviceType::discrete)
                    .set_required_features(requiredFeatures)
                    .select();
    if (!pdev) {
        vkb::destroy_instance(m_instance);
        throw std::runtime_error("could not pick physical device: " + pdev.error().message());
    }
    m_physicalDevice = pdev.value();

    vkb::DeviceBuilder devBuild(m_physicalDevice);
    auto dev = devBuild.build();
    if (!dev) {
        vkb::destroy_instance(m_instance);
        throw std::runtime_error("could not build logical device: " + dev.error().message());
    }
    m_device = dev.value();

    auto computeQueueFamily = std::find_if(
        m_device.queue_families.cbegin(), m_device.queue_families.cend(), [](const auto& family) {
            return (family.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0;
        });
    if (computeQueueFamily == m_device.queue_families.cend()) {
        vkb::destroy_device(m_device);
        vkb::destroy_instance(m_instance);
        throw std::runtime_error("could not acquire compute queue: "
                                 + vkb::make_error_code(vkb::QueueError::compute_unavailable).message());
    }

    m_computeQueue = vkb::detail::get_queue(
        m_device.device, static_cast<uint32_t>(computeQueueFamily - m_device.queue_families.cbegin()));
}

VulkanContext::~VulkanContext() {
    vkb::destroy_device(m_device);
    vkb::destroy_instance(m_instance);
}

const vkb::Instance& VulkanContext::instance() const {
    return m_instance;
}

const vkb::PhysicalDevice& VulkanContext::physicalDevice() const {
    return m_physicalDevice;
}

const vkb::Device& VulkanContext::device() {
    return m_device;
}

VkQueue VulkanContext::compute() const {
    return m_computeQueue;
}

} // namespace vk_util
