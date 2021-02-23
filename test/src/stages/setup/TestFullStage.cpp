#include <iostream>

#include <gtest/gtest.h>

#include <gpu/host-interface/main/TriangleSetupAPI.h>

#include "../SimpleVulkanFixture.h"

#include "../../utils/CommonDebugMessengers.h"
#include "../../utils/ManagedVulkanResource.h"
#include "../../utils/VulkanContext.h"

using namespace utils;

class TriangleSetup : public SimpleVulkanFixture {
protected:
    void SetUp() override {
        SimpleVulkanFixture::SetUp();
        m_setup = std::make_unique<ManagedVulkanResource<vkr::gpu::TriangleSetupAPI>>(context().device(), nullptr);
        m_setupRunner = std::make_unique<vk::UniquePipeline>(
            context().device().createComputePipelineUnique(nullptr, setup()->describeRunner()));
    }

    void TearDown() override {
        m_setupRunner.reset();
        m_setup.reset();
        SimpleVulkanFixture::TearDown();
    }

    ManagedVulkanResource<vkr::gpu::TriangleSetupAPI>& setup() {
        return *m_setup;
    }

    vk::UniquePipeline& setupRunner() {
        return *m_setupRunner;
    }

private:
    std::unique_ptr<ManagedVulkanResource<vkr::gpu::TriangleSetupAPI>> m_setup;
    std::unique_ptr<vk::UniquePipeline> m_setupRunner;
};

TEST_F(TriangleSetup, StageSanity) {
}
