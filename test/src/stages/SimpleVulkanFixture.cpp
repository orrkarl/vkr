#include "SimpleVulkanFixture.h"

void SimpleVulkanFixture::SetUp() {
    m_validationErrorCounter->reset();
    m_currentContext = std::make_unique<utils::VulkanContext>(std::unique_ptr<utils::ChainedDebugMessenger>(
        new utils::ChainedDebugMessenger({ *m_validationErrorCounter, *m_printingMessenger })));
}

void SimpleVulkanFixture::TearDown() {
    m_currentContext.reset();
}

utils::VulkanContext& SimpleVulkanFixture::context() {
    return *m_currentContext;
}
