ADD_EXECUTABLE(vkr-tests src/Main.cpp src/example/TestFail.cpp)

TARGET_COMPILE_OPTIONS(vkr-tests PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror>
        )

TARGET_LINK_LIBRARIES(vkr-tests PRIVATE Catch2::Catch2 PRIVATE vkr-backend)

catch_discover_tests(vkr-tests)
