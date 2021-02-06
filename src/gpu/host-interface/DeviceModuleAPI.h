#pragma once

#include <array>

#include <vulkan/vulkan.h>

namespace vkr {
namespace gpu {

///
/// struct APITraits {
///     static const size_t MODULE_SIZE = <size of module code in uint32_t>;
///     static const uint32_t* MODULE_DATA = <pointer to module data>;
///
///     static const std::array<VkDescriptorSetLayoutBinding, TOTAL_BINDINGS_COUNT> BINDINGS = <all descriptor
///     bindings>;
///
/// 	static const std::array<uint32_t, ARG_SETS_COUNT> SETS = <sizes of each descriptor set in
///     BINDINGS array in order>;
///
/// 	static const std::array<VkPushConstantRange, PUSH_CONSTANTS_COUNT> PUSH_ARGS
///     = <description of all push constants>;
///
/// 	static const std::array<uint32_t, 3> GROUP_SIZES = <3d size of dispatch group>
/// };
///

template <typename APILayout>
class DeviceModuleAPI {
public:
    using Layout = APILayout;

    using ArgumentSetsLayout = std::array<VkDescriptorSetLayout, APILayout::SETS.size()>;

    explicit DeviceModuleAPI();

    VkResult init(VkDevice dev, const VkAllocationCallbacks* allocator);
    void destroy(VkDevice dev, const VkAllocationCallbacks* allocator);

    ArgumentSetsLayout describeArguments();
    VkComputePipelineCreateInfo describeRunner();
    VkPipelineLayout runnerLayout();

    std::array<uint32_t, 3> dispatchGroupSizes() const;

protected:
    VkShaderModule m_code;
    ArgumentSetsLayout m_argsDesc;
    VkPipelineLayout m_runnerDesc;
};

} // namespace gpu
} // namespace vkr

#include "DeviceModuleAPI.inl"
