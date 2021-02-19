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

} // namespace utils
