#include "APIUtils.h"

namespace vkr {
namespace gpu {

VkWriteDescriptorSet describeSimpleBufferUpdate(VkDescriptorSet set,
                                                uint32_t binding,
                                                VkDescriptorType bufferType,
                                                const VkDescriptorBufferInfo& bufferInfo) {
    return { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
             nullptr,
             set,
             binding,
             0,
             1,
             bufferType,
             nullptr,
             &bufferInfo,
             nullptr };
}

}
}
