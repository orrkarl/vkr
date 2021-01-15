# Main VulkanRasterizer libraries 

ADD_LIBRARY(vkr-backend STATIC
        TriangleSetupContext.cpp)

TARGET_COMPILE_OPTIONS(vkr-backend PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
		$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror>
		)

ADD_SUBDIRECTORY(gpu)

TARGET_LINK_LIBRARIES(vkr-backend
    PUBLIC Vulkan::Vulkan
    PRIVATE vkr-gpu
    INTERFACE vkr-global-compiler-opts)
TARGET_INCLUDE_DIRECTORIES(vkr-backend INTERFACE "../src")
