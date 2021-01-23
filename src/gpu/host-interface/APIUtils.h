#pragma once

#include <vulkan/vulkan.h>

namespace vkr {
namespace gpu {

VkWriteDescriptorSet describeSimpleBufferUpdate(VkDescriptorSet set,
                                                uint32_t binding,
                                                VkDescriptorType bufferType,
                                                const VkDescriptorBufferInfo& bufferInfo);

}
}
