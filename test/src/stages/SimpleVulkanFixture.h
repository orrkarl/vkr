#pragma once

#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "../utils/CommonDebugMessengers.h"
#include "../utils/MemoryUtils.h"
#include "../utils/VulkanContext.h"

class SimpleVulkanFixture : public ::testing::Test {
protected:
    void SetUp();

    void TearDown();

    utils::VulkanContext& context();

    vk::UniqueDescriptorPool createDescriptorPool(uint32_t maxSetCount,
                                                  const std::vector<vk::DescriptorPoolSize>& poolCaps);

    template <typename T>
    std::vector<T> readDeviceMemory(vk::DeviceMemory memory, const utils::TypedRegionDescriptor<T>& region);

private:
    utils::OstreamLoggingMessenger m_printingMessenger { std::cout,
                                                         vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning };
    utils::SeverityCountMessenger m_validationErrorCounter { vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                                                             | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError };

    std::unique_ptr<utils::VulkanContext> m_currentContext;
};

#include "SimpleVulkanFixture.inl"
