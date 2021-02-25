#pragma once

#include <fstream>
#include <ostream>

#include "IDebugMessenger.h"

namespace utils {

class OstreamLoggingMessenger : public IDebugMessenger {
public:
    OstreamLoggingMessenger(std::ostream& output, vk::DebugUtilsMessageSeverityFlagBitsEXT minSeverity);

protected:
    void debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                       vk::DebugUtilsMessageTypeFlagsEXT type,
                       const vk::DebugUtilsMessengerCallbackDataEXT& callbackData) override;

private:
    std::ostream& m_output;
};

class FileLoggingMessenger : public IDebugMessenger {
public:
    FileLoggingMessenger(const std::string& path);

protected:
    void debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                       vk::DebugUtilsMessageTypeFlagsEXT type,
                       const vk::DebugUtilsMessengerCallbackDataEXT& callbackData) override;

private:
    std::ofstream m_output;
};

class SeverityCountMessenger : public IDebugMessenger {
public:
    SeverityCountMessenger(vk::DebugUtilsMessageSeverityFlagsEXT severities);

    uint64_t count() const;

protected:
    void debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                       vk::DebugUtilsMessageTypeFlagsEXT type,
                       const vk::DebugUtilsMessengerCallbackDataEXT& callbackData) override;

private:
    uint64_t m_counter;
};

class ChainedDebugMessenger : public IDebugMessenger {
public:
    ChainedDebugMessenger(std::vector<std::reference_wrapper<IDebugMessenger>> messengers);

protected:
    void debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                       vk::DebugUtilsMessageTypeFlagsEXT type,
                       const vk::DebugUtilsMessengerCallbackDataEXT& callbackData) override;

private:
    std::vector<vk::DebugUtilsMessengerCreateInfoEXT> m_messengers;
};

} // namespace utils
