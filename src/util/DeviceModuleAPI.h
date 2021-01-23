#pragma once

#include <array>

#include <vulkan/vulkan.h>

namespace vkr {
namespace gpu {
namespace detail {

template <typename APITraits>
class DeviceModuleAPI {
public:
    using ArgumentSetsLayout = std::array<VkDescriptorSetLayout, APITraits::ARG_SETS.size()>;

    explicit DeviceModuleAPI();

    VkResult init(VkDevice dev, const VkAllocationCallbacks* allocator);
    void destroy(VkDevice dev, const VkAllocationCallbacks* allocator);

    ArgumentSetsLayout describeArguments();
    VkComputePipelineCreateInfo describeRunner();

protected:
    VkShaderModule m_code;
    ArgumentSetsLayout m_argsDesc;
    VkPipelineLayout m_runnerDesc;
};

}
}
}

#include "DeviceModuleAPI.inl"
