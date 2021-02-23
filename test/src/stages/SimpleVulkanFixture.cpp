#include "SimpleVulkanFixture.h"

void SimpleVulkanFixture::SetUp() {
    m_currentContext = std::make_unique<utils::VulkanContext>();
}

void SimpleVulkanFixture::TearDown() {
    m_currentContext.reset();
}

utils::VulkanContext& SimpleVulkanFixture::context() {
    return *m_currentContext;
}
