#include "CommonDebugMessengers.h"

namespace utils {

OstreamLoggingMessenger::OstreamLoggingMessenger(std::ostream& output,
                                                 vk::DebugUtilsMessageSeverityFlagBitsEXT minSeverity)
    : IDebugMessenger(minSeverityToBitmap(minSeverity)), m_output(output) {
}

void OstreamLoggingMessenger::debugCallback(vk::DebugUtilsMessageSeverityFlagsEXT severity,
                                            vk::DebugUtilsMessageTypeFlagsEXT type,
                                            const vk::DebugUtilsMessengerCallbackDataEXT& callbackData) {
    m_output << stringifyDebugMessage(severity, type, callbackData) << std::endl;
}

FileLoggingMessenger::FileLoggingMessenger(const std::string& path) : m_output(path, std::ios::trunc | std::ios::out) {
}

void FileLoggingMessenger::debugCallback(vk::DebugUtilsMessageSeverityFlagsEXT severity,
                                         vk::DebugUtilsMessageTypeFlagsEXT type,
                                         const vk::DebugUtilsMessengerCallbackDataEXT& callbackData) {
    m_output << stringifyDebugMessage(severity, type, callbackData) << std::endl;
}

SeverityCountMessenger::SeverityCountMessenger(uint64_t& counter, vk::DebugUtilsMessageSeverityFlagsEXT severities)
    : IDebugMessenger(severities), m_counter(counter) {
}

void SeverityCountMessenger::debugCallback(vk::DebugUtilsMessageSeverityFlagsEXT severity,
                                           vk::DebugUtilsMessageTypeFlagsEXT type,
                                           const vk::DebugUtilsMessengerCallbackDataEXT& callbackData) {
    m_counter++;
}

} // namespace utils
