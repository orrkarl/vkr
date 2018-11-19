#include <string>
#include <assert.h>
#include <vulkan/vulkan.h>

#include <iostream>
#include <vector>

#pragma once

const char* g_AppName = "my vulkan app"; 
const std::vector<const char*> g_validationLayers = {
    "VK_LAYER_LUNARG_standard_validation"
};

//#ifdef DEBUG
    const bool g_enableValidationLayers = true;
//#else
//    const bool g_enableValidationLayers = false;
//#endif


