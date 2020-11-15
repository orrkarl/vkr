INCLUDE(GoogleTest)

ADD_EXECUTABLE(NrasterTests src/base/TestSimpleUsage.cpp)
TARGET_LINK_LIBRARIES(NrasterTests gtest gmock gtest_main)
gtest_discover_tests(NrasterTests)
