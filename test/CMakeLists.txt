INCLUDE(GoogleTest)

ADD_EXECUTABLE(NrasterTests src/base/TestSimpleUsage.cpp)
TARGET_LINK_LIBRARIES(NrasterTests gtest gmock gtest_main NrasterBackend)
TARGET_INCLUDE_DIRECTORIES(NrasterTests PRIVATE "../src")

gtest_discover_tests(NrasterTests)
