#pragma once

#include <ostream>

#include "IDebugMessenger.h"

namespace utils {

class OstreamLoggingMessenger : public IDebugMessenger {
public:
    OstreamLoggingMessenger(std::ostream& output, vk::DebugUtilsMessageSeverityFlagBitsEXT minSeverity);

protected:
    void debugCallback(vk::DebugUtilsMessageSeverityFlagsEXT severity,
                       vk::DebugUtilsMessageTypeFlagsEXT type,
                       const vk::DebugUtilsMessengerCallbackDataEXT* callbackData) override;

private:
    std::ostream& m_output;
};

} // namespace utils
