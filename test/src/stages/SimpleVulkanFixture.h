#pragma once

#include <memory>

#include <gtest/gtest.h>

#include "../utils/CommonDebugMessengers.h"
#include "../utils/VulkanContext.h"

class SimpleVulkanFixture : public ::testing::Test {
protected:
    void SetUp();

    void TearDown();

    utils::VulkanContext& context();

private:
    std::unique_ptr<utils::OstreamLoggingMessenger> m_printingMessenger { new utils::OstreamLoggingMessenger(
        std::cout, vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) };
    std::unique_ptr<utils::SeverityCountMessenger> m_validationErrorCounter { new utils::SeverityCountMessenger(
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError) };

    std::unique_ptr<utils::VulkanContext> m_currentContext;
};
