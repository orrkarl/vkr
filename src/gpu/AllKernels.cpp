#include "AllKernels.h"

namespace vkr {
namespace gpu {

VkShaderModuleCreateInfo describeTriangleSetup() {
    return VkShaderModuleCreateInfo { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, nullptr, 0,
                                      device_side::TriangleSetup_comp_count * sizeof(uint32_t),
                                      device_side::TriangleSetup_comp };
}

} // gpu
} // vkr
