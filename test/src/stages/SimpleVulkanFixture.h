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
    utils::OstreamLoggingMessenger m_printingMessenger { std::cout,
                                                         vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning };
    utils::SeverityCountMessenger m_validationErrorCounter { vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                                                             | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError };

    std::unique_ptr<utils::VulkanContext> m_currentContext;
};
