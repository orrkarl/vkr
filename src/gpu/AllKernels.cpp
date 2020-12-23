#include "AllKernels.h"

namespace vkr {
namespace gpu {

vk::ShaderModuleCreateInfo describeSimple() {
    return vk::ShaderModuleCreateInfo(vk::ShaderModuleCreateFlags(),
                                      device_side::Simple_comp_count * sizeof(uint32_t),
                                      device_side::Simple_comp);
}

vk::ShaderModuleCreateInfo describeTriangleSetup() {
    return vk::ShaderModuleCreateInfo(vk::ShaderModuleCreateFlags(),
                                      device_side::TriangleSetup_comp_count * sizeof(uint32_t),
                                      device_side::TriangleSetup_comp);
}

} // gpu
} // vkr
