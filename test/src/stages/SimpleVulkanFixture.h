#pragma once

#include <memory>

#include <gtest/gtest.h>

#include "../utils/VulkanContext.h"

class SimpleVulkanFixture : public ::testing::Test
{
protected:
    void SetUp();

    void TearDown();

    utils::VulkanContext& context();

private:
    std::unique_ptr<utils::VulkanContext> m_currentContext;
};

