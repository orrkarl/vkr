ADD_EXECUTABLE(NrasterTests src/Main.cpp src/util/Prettify.cpp src/base/TestSimpleUsage.cpp)
TARGET_COMPILE_OPTIONS(NrasterTests PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror>
        )

TARGET_LINK_LIBRARIES(NrasterTests Catch2::Catch2 NrasterBackend)

catch_discover_tests(NrasterTests)
