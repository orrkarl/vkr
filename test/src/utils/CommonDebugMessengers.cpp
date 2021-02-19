#include "CommonDebugMessengers.h"

namespace utils {

OstreamLoggingMessenger::OstreamLoggingMessenger(std::ostream& output,
                                                 vk::DebugUtilsMessageSeverityFlagBitsEXT minSeverity)
    : IDebugMessenger(minSeverityToBitmap(minSeverity)), m_output(output) {
}

void OstreamLoggingMessenger::debugCallback(vk::DebugUtilsMessageSeverityFlagsEXT severity,
                                            vk::DebugUtilsMessageTypeFlagsEXT type,
                                            const vk::DebugUtilsMessengerCallbackDataEXT* callbackData) {
    m_output << stringifyDebugMessage(severity, type, callbackData) << std::endl;
}

FileLoggingMessenger::FileLoggingMessenger(const std::string& path,
                                           vk::DebugUtilsMessageSeverityFlagBitsEXT minSeverity)
    : IDebugMessenger(minSeverityToBitmap(minSeverity)), m_output(path, std::ios::trunc | std::ios::out) {
}

void FileLoggingMessenger::debugCallback(vk::DebugUtilsMessageSeverityFlagsEXT severity,
                                         vk::DebugUtilsMessageTypeFlagsEXT type,
                                         const vk::DebugUtilsMessengerCallbackDataEXT* callbackData) {
    m_output << stringifyDebugMessage(severity, type, callbackData) << std::endl;
}

SeverityCountMessenger::SeverityCountMessenger(vk::DebugUtilsMessageSeverityFlagsEXT severities)
    : IDebugMessenger(severities), m_counter(0) {
}

uint64_t SeverityCountMessenger::count() const {
    return m_counter;
}

void SeverityCountMessenger::debugCallback(vk::DebugUtilsMessageSeverityFlagsEXT severity,
                                           vk::DebugUtilsMessageTypeFlagsEXT type,
                                           const vk::DebugUtilsMessengerCallbackDataEXT* callbackData) {
    m_counter++;
}

} // namespace utils
