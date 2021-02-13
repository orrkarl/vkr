# Main VulkanRasterizer libraries 

ADD_LIBRARY(vkr-backend STATIC
    util/Dummy.cpp)

ADD_SUBDIRECTORY(gpu)

TARGET_LINK_LIBRARIES(vkr-backend
    PUBLIC vkr-gpu
    PRIVATE Vulkan::Vulkan
    PRIVATE vkr-global-compiler-opts)
TARGET_INCLUDE_DIRECTORIES(vkr-backend INTERFACE "../src")
