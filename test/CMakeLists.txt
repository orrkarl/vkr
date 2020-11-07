ADD_EXECUTABLE(NrasterTest src/tests.cpp)

TARGET_LINK_LIBRARIES(NrasterTest
	PRIVATE gtest
	PRIVATE gmock)

TARGET_INCLUDE_DIRECTORIES(NrasterTest 
	PRIVATE ${gtest_INCLUDE_DIRS})

if(WIN32)
    # Copy DLL files to output directory
    ADD_CUSTOM_COMMAND(TARGET NrasterTest PRE_LINK
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_BINARY_DIR}/project/Nraster.dll"             
    $<TARGET_FILE_DIR:NrasterTest>)
endif()
