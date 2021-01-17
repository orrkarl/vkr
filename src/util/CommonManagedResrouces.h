#pragma once

#include "LocalManagedHandle.h"

namespace vkr {

template <>
struct VkResourceTraits<VkShaderModule> {
    static constexpr auto destroy = vkDestroyShaderModule;
};

template <>
struct VkResourceTraits<VkDescriptorSetLayout> {
    static constexpr auto destroy = vkDestroyDescriptorSetLayout;
};

template <>
struct VkResourceTraits<VkPipelineLayout> {
    static constexpr auto destroy = vkDestroyPipelineLayout;
};

template <>
struct VkResourceTraits<VkPipeline> {
    static constexpr auto destroy = vkDestroyPipeline;
};

}
