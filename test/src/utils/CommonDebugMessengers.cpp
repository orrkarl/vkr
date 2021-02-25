#include "CommonDebugMessengers.h"

namespace utils {

OstreamLoggingMessenger::OstreamLoggingMessenger(std::ostream& output,
                                                 vk::DebugUtilsMessageSeverityFlagBitsEXT minSeverity)
    : IDebugMessenger(minSeverityToBitmap(minSeverity)), m_output(output) {
}

void OstreamLoggingMessenger::debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                            vk::DebugUtilsMessageTypeFlagsEXT type,
                                            const vk::DebugUtilsMessengerCallbackDataEXT& callbackData) {
    m_output << stringifyDebugMessage(severity, type, callbackData) << std::endl;
}

FileLoggingMessenger::FileLoggingMessenger(const std::string& path) : m_output(path, std::ios::trunc | std::ios::out) {
}

void FileLoggingMessenger::debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                         vk::DebugUtilsMessageTypeFlagsEXT type,
                                         const vk::DebugUtilsMessengerCallbackDataEXT& callbackData) {
    m_output << stringifyDebugMessage(severity, type, callbackData) << std::endl;
}

SeverityCountMessenger::SeverityCountMessenger(vk::DebugUtilsMessageSeverityFlagsEXT severities)
    : IDebugMessenger(severities), m_counter(0) {
}

uint64_t SeverityCountMessenger::count() const {
    return m_counter;
}

void SeverityCountMessenger::debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                           vk::DebugUtilsMessageTypeFlagsEXT type,
                                           const vk::DebugUtilsMessengerCallbackDataEXT& callbackData) {
    m_counter++;
}

ChainedDebugMessenger::ChainedDebugMessenger(std::vector<std::reference_wrapper<IDebugMessenger>> messengers)
    : m_messengers(messengers.size()) {
    std::transform(messengers.begin(), messengers.end(), m_messengers.begin(), [](auto& messenger) {
        return messenger.get().describeMessenger();
    });
}

void ChainedDebugMessenger::debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                          vk::DebugUtilsMessageTypeFlagsEXT type,
                                          const vk::DebugUtilsMessengerCallbackDataEXT& callbackData) {
    for (auto& messenger : m_messengers) {
        if ((messenger.messageSeverity & severity) && (messenger.messageType & type)) {
            messenger.pfnUserCallback(static_cast<VkDebugUtilsMessageSeverityFlagBitsEXT>(severity),
                                      static_cast<VkDebugUtilsMessageTypeFlagsEXT>(type),
                                      reinterpret_cast<const VkDebugUtilsMessengerCallbackDataEXT*>(&callbackData),
                                      messenger.pUserData);
        }
    }
}

} // namespace utils
