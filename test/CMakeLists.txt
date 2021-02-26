include(GoogleTest)

ADD_EXECUTABLE(vkr-tests
        ${vk-bootstrap_SOURCE_DIR}/src/VkBootstrap.cpp
        src/utils/MemoryUtils.cpp
        src/utils/VulkanContext.cpp
        src/utils/IDebugMessenger.cpp
        src/utils/CommonDebugMessengers.cpp
        src/stages/SimpleVulkanFixture.cpp
        src/stages/setup/TestFullStage.cpp
        src/stages/setup/TestClipping.cpp)

TARGET_LINK_LIBRARIES(vkr-tests
        PRIVATE ${CMAKE_DL_LIBS}
        PRIVATE gtest_main
        PRIVATE gmock
        PRIVATE vkr-backend
        PRIVATE vkr-gpu-tests)

TARGET_INCLUDE_DIRECTORIES(vkr-tests
        PRIVATE ${vk-bootstrap_SOURCE_DIR}/src)

gtest_discover_tests(vkr-tests)
