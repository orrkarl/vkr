# Main VulkanRasterizer libraries 

ADD_LIBRARY(vkr-backend STATIC
        gpu/AllKernels.cpp
		)

TARGET_COMPILE_OPTIONS(vkr-backend PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
		$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror>
		)

ADD_SPIRV_LIBRARY(
    TARGET vkr-kernel-resources
    NAMESPACE device_side
    OUTPUT_PATH "gpu/generated"
    HEADER_NAME "AllKernels.h"
    SOURCES "gpu/Simple.comp")

TARGET_LINK_LIBRARIES(vkr-backend PUBLIC Vulkan::Vulkan PRIVATE vkr-kernel-resources)
TARGET_INCLUDE_DIRECTORIES(vkr-backend INTERFACE "../src")
