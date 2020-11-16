ADD_EXECUTABLE(NrasterTests src/Main.cpp src/base/TestSimpleUsage.cpp)

TARGET_LINK_LIBRARIES(NrasterTests Catch2::Catch2 NrasterBackend)

catch_discover_tests(NrasterTests)
