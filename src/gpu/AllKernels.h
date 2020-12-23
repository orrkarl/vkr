#pragma once

#include <vulkan/vulkan.hpp>

#include "generated/AllKernels.h"
#include "shared/compatibility.h"

namespace vkr {
namespace gpu {

vk::ShaderModuleCreateInfo describeSimple();

} // gpu
} // vkr
