#include "IDebugMessenger.h"

#include <sstream>
#include <type_traits>

namespace utils {

vk::Bool32 IDebugMessenger::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                          VkDebugUtilsMessageTypeFlagsEXT messageType,
                                          const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                          void* pUserData) {
    reinterpret_cast<IDebugMessenger*>(pUserData)->debugCallback(
        vk::DebugUtilsMessageSeverityFlagBitsEXT(messageSeverity),
        vk::DebugUtilsMessageTypeFlagsEXT(messageType),
        *reinterpret_cast<const vk::DebugUtilsMessengerCallbackDataEXT*>(pCallbackData));
    return VK_FALSE;
}

IDebugMessenger::IDebugMessenger(vk::DebugUtilsMessageSeverityFlagsEXT severities,
                                 vk::DebugUtilsMessageTypeFlagsEXT types)
    : m_severities(severities), m_types(types) {
}

vk::DebugUtilsMessengerCreateInfoEXT IDebugMessenger::describeMessenger() {
    return vk::DebugUtilsMessengerCreateInfoEXT(
        vk::DebugUtilsMessengerCreateFlagsEXT(), m_severities, m_types, &IDebugMessenger::debugCallback, this);
}

std::string IDebugMessenger::stringifyDebugMessage(vk::DebugUtilsMessageSeverityFlagsEXT severity,
                                                   vk::DebugUtilsMessageTypeFlagsEXT type,
                                                   const vk::DebugUtilsMessengerCallbackDataEXT& callbackData) {
    std::stringstream ss;
    ss << "==========================================================================" << std::endl;
    ss << "DEBUG Callback Message: " << callbackData.pMessageIdName << " (" << callbackData.messageIdNumber << ")"
       << std::endl;
    ss << "\tSEVERITIES = " << vk::to_string(severity) << std::endl;
    ss << "\tTYPES = " << vk::to_string(type) << std::endl;
    ss << callbackData.pMessage << std::endl;
    ss << "==========================================================================" << std::endl;
    return ss.str();
}

vk::DebugUtilsMessageSeverityFlagsEXT IDebugMessenger::minSeverityToBitmap(
    vk::DebugUtilsMessageSeverityFlagBitsEXT min) {
    auto arithmeticSeverity = static_cast<std::underlying_type_t<vk::DebugUtilsMessageSeverityFlagBitsEXT>>(min);
    return ALL_SEVERITIES & ~(vk::DebugUtilsMessageSeverityFlagsEXT(arithmeticSeverity - 1));
}

DebugMessengerRegisterGuard::DebugMessengerRegisterGuard(vk::Instance inst,
                                                         const vk::DispatchLoaderDynamic& dynamics,
                                                         IDebugMessenger& messenger,
                                                         vk::Optional<const vk::AllocationCallbacks> allocator)
    : m_messengerHandle(inst.createDebugUtilsMessengerEXTUnique(messenger.describeMessenger(), allocator, dynamics)) {
}

} // namespace utils
