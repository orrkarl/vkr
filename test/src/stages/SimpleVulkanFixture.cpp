#include "SimpleVulkanFixture.h"

void SimpleVulkanFixture::SetUp() {
    m_validationErrorCounter.resetCounter();
    m_currentContext = std::make_unique<utils::VulkanContext>(std::unique_ptr<utils::ChainedDebugMessenger>(
        new utils::ChainedDebugMessenger({ m_validationErrorCounter, m_printingMessenger })));
}

void SimpleVulkanFixture::TearDown() {
    m_currentContext.reset();
    ASSERT_EQ(m_validationErrorCounter.count(), 0) << "Some validation errors occured during the run of this test";
}

utils::VulkanContext& SimpleVulkanFixture::context() {
    return *m_currentContext;
}

vk::UniqueDescriptorPool SimpleVulkanFixture::createDescriptorPool(
    uint32_t maxSetCount, const std::vector<vk::DescriptorPoolSize>& poolCaps) {
    return m_currentContext->device().createDescriptorPoolUnique(
        { vk::DescriptorPoolCreateFlags(), maxSetCount, static_cast<uint32_t>(poolCaps.size()), poolCaps.data() });
}
