ADD_EXECUTABLE(vkr-tests
        ${vk-bootstrap_SOURCE_DIR}/src/VkBootstrap.cpp
        src/example/TestFail.cpp)

TARGET_LINK_LIBRARIES(vkr-tests
        PRIVATE ${CMAKE_DL_LIBS}
        PRIVATE Catch2WithMain
        PRIVATE vkr-backend
        INTERFACE vkr-global-compiler-opts)

TARGET_INCLUDE_DIRECTORIES(vkr-tests
        PRIVATE ${vk-bootstrap_SOURCE_DIR}/src)

catch_discover_tests(vkr-tests)
