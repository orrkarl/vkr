#include "AllKernels.h"

namespace vkr {
namespace gpu {

vk::ShaderModuleCreateInfo describeSimple() {
    return vk::ShaderModuleCreateInfo(vk::ShaderModuleCreateFlags(), ::device_side::Simple_comp_size, ::device_side::Simple_comp);
}


} // gpu
} // vkr
