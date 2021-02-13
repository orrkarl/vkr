#include "IDebugMessenger.h"

namespace utils {

vk::Bool32 IDebugMessenger::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                          VkDebugUtilsMessageTypeFlagsEXT messageType,
                                          const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                          void* pUserData) {
    reinterpret_cast<IDebugMessenger*>(pUserData)->debugCallback(messageSeverity, messageType, pCallbackData);
    return VK_FALSE;
}

vk::DebugUtilsMessengerCreateInfoEXT IDebugMessenger::describeMessenger(
    vk::DebugUtilsMessageSeverityFlagsEXT allowedSeverity, vk::DebugUtilsMessageTypeFlagsEXT allowedTypes) {
    return vk::DebugUtilsMessengerCreateInfoEXT(vk::DebugUtilsMessengerCreateFlagsEXT(),
                                                allowedSeverity,
                                                allowedTypes,
                                                &IDebugMessenger::debugCallback,
                                                this);
}

DebugMessengerRegisterGuard::DebugMessengerRegisterGuard(
    vk::Instance inst,
    vk::DebugUtilsMessageSeverityFlagsEXT allowedSeverities,
    vk::DebugUtilsMessageTypeFlagsEXT allowedTypes,
    IDebugMessenger& messenger,
    vk::Optional<const vk::AllocationCallbacks> allocator)
    : m_messengerHandle(
        inst.createDebugUtilsMessengerEXTUnique(messenger.describeMessenger(allowedSeverities, allowedTypes),
                                                allocator,
                                                vk::DispatchLoaderDynamic(inst, vkGetInstanceProcAddr)))
    , m_userMessengerObj(messenger) {
}

} // namespace utils
