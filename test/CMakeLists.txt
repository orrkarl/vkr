ADD_EXECUTABLE(vkr-tests
        ${vk-bootstrap_SOURCE_DIR}/src/VkBootstrap.cpp
        src/utils/MemoryUtils.cpp
        src/utils/VulkanContext.cpp
        src/stages/setup/TestFullStage.cpp
        src/stages/setup/TestClipping.cpp)

TARGET_LINK_LIBRARIES(vkr-tests
        PRIVATE ${CMAKE_DL_LIBS}
        PRIVATE Catch2WithMain
        PRIVATE vkr-backend
        PRIVATE vkr-gpu-tests
        INTERFACE vkr-global-compiler-opts)

TARGET_INCLUDE_DIRECTORIES(vkr-tests
        PRIVATE ${vk-bootstrap_SOURCE_DIR}/src)

catch_discover_tests(vkr-tests)
