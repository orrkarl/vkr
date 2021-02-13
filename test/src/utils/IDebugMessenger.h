#pragma once

#include <vulkan/vulkan.hpp>

namespace utils {

class IDebugMessenger {
public:
    virtual ~IDebugMessenger() = default;

    vk::DebugUtilsMessengerCreateInfoEXT describeMessenger(
        vk::DebugUtilsMessageSeverityFlagsEXT allowedSeverity = vk::DebugUtilsMessageSeverityFlagsEXT(
            vk::FlagTraits<vk::DebugUtilsMessageSeverityFlagBitsEXT>::allFlags),
        vk::DebugUtilsMessageTypeFlagsEXT allowedTypes = vk::DebugUtilsMessageTypeFlagsEXT(
            vk::FlagTraits<vk::DebugUtilsMessageTypeFlagBitsEXT>::allFlags));

protected:
    virtual void debugCallback(VkDebugUtilsMessageSeverityFlagsEXT severity,
                               VkDebugUtilsMessageTypeFlagsEXT type,
                               const VkDebugUtilsMessengerCallbackDataEXT* callbackData)
        = 0;

private:
    static VKAPI_ATTR vk::Bool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                  void* pUserData);

    // since a pointer to the messenger is passed to vulkan, moving/copying is invalid
    IDebugMessenger(const IDebugMessenger&) = delete;
    IDebugMessenger& operator=(const IDebugMessenger&) = delete;
    IDebugMessenger(IDebugMessenger&&) = delete;
    IDebugMessenger& operator=(IDebugMessenger&&) = delete;
};

class DebugMessengerRegisterGuard {
public:
    DebugMessengerRegisterGuard(vk::Instance inst,
                                vk::DebugUtilsMessageSeverityFlagsEXT allowedSeverities,
                                vk::DebugUtilsMessageTypeFlagsEXT allowedTypes,
                                IDebugMessenger& messenger,
                                vk::Optional<const vk::AllocationCallbacks> allocator = nullptr);

private:
    vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> m_messengerHandle;
    IDebugMessenger& m_userMessengerObj;
};

} // namespace utils
