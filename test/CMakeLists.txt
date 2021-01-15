ADD_EXECUTABLE(vkr-tests src/Main.cpp src/example/TestFail.cpp)

TARGET_LINK_LIBRARIES(vkr-tests
        PRIVATE Catch2::Catch2
        PRIVATE vkr-backend
        PRIVATE vk-bootstrap::vk-bootstrap
        INTERFACE vkr-global-compiler-opts)

catch_discover_tests(vkr-tests)
