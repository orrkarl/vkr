ADD_EXECUTABLE(NrasterTest 
	src/tests.cpp
	src/includes.cpp)

TARGET_LINK_LIBRARIES(NrasterTest 
	PRIVATE Nraster
	PRIVATE gtest 
	PRIVATE gmock)

TARGET_INCLUDE_DIRECTORIES(NrasterTest 
	PRIVATE ${NRASTER_ALL_INCLUDES}
	PRIVATE ${gtest_INCLUDE_DIRS}
	PRIVATE ${OpenCL_INCLUDE_DIRS})

if(WIN32)
    # Copy DLL files to output directory
    ADD_CUSTOM_COMMAND(TARGET NrasterTest PRE_LINK
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_BINARY_DIR}/lib/Nraster.dll"             
    $<TARGET_FILE_DIR:NrasterTest>)
endif()
