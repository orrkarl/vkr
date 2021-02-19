#pragma once

#include <vulkan/vulkan.hpp>

namespace utils {

class IDebugMessenger {
public:
    static constexpr vk::DebugUtilsMessageSeverityFlagsEXT ALL_SEVERITIES = vk::DebugUtilsMessageSeverityFlagsEXT(
        vk::FlagTraits<vk::DebugUtilsMessageSeverityFlagBitsEXT>::allFlags);

    static constexpr vk::DebugUtilsMessageTypeFlagsEXT ALL_TYPES = vk::DebugUtilsMessageTypeFlagsEXT(
        vk::FlagTraits<vk::DebugUtilsMessageTypeFlagBitsEXT>::allFlags);

    IDebugMessenger(vk::DebugUtilsMessageSeverityFlagsEXT severities = ALL_SEVERITIES,
                    vk::DebugUtilsMessageTypeFlagsEXT types = ALL_TYPES);

    virtual ~IDebugMessenger() = default;

    vk::DebugUtilsMessengerCreateInfoEXT describeMessenger();

protected:
    static std::string stringifyDebugMessage(vk::DebugUtilsMessageSeverityFlagsEXT severity,
                                             vk::DebugUtilsMessageTypeFlagsEXT type,
                                             const vk::DebugUtilsMessengerCallbackDataEXT* callbackData);

    static vk::DebugUtilsMessageSeverityFlagsEXT minSeverityToBitmap(vk::DebugUtilsMessageSeverityFlagBitsEXT min);

    virtual void debugCallback(vk::DebugUtilsMessageSeverityFlagsEXT severity,
                               vk::DebugUtilsMessageTypeFlagsEXT type,
                               const vk::DebugUtilsMessengerCallbackDataEXT* callbackData)
        = 0;

private:
    static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                          VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                          const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                          void* pUserData);

    // since a pointer to the messenger is passed to vulkan, moving/copying is invalid
    IDebugMessenger(const IDebugMessenger&) = delete;
    IDebugMessenger& operator=(const IDebugMessenger&) = delete;
    IDebugMessenger(IDebugMessenger&&) = delete;
    IDebugMessenger& operator=(IDebugMessenger&&) = delete;

    vk::DebugUtilsMessageSeverityFlagsEXT m_severities;
    vk::DebugUtilsMessageTypeFlagsEXT m_types;
};

class DebugMessengerRegisterGuard {
public:
    DebugMessengerRegisterGuard(vk::Instance inst,
                                const vk::DispatchLoaderDynamic& dynamics,
                                IDebugMessenger& messenger,
                                vk::Optional<const vk::AllocationCallbacks> allocator = nullptr);

private:
    vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> m_messengerHandle;
};

} // namespace utils
