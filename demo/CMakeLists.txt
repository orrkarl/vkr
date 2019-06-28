CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

INCLUDE_DIRECTORIES(
	PUBLIC inc
	PUBLIC ${NRASTER_ALL_INCLUDES}
	PUBLIC ${OpenGL_INCLUDE_DIRS}
	PUBLIC ${glew_INCLUDE_DIRS}
	PUBLIC ${glfw_INCLUDE_DIRS})

ADD_LIBRARY(NrasterDemoUtils src/utils.cpp src/linalg.cpp)

TARGET_LINK_LIBRARIES(NrasterDemoUtils 
	PUBLIC Nraster
	PUBLIC opengl32
	PUBLIC glfw
	PUBLIC glew)

ADD_EXECUTABLE(Triangle3d src/triangle3d.cpp)
TARGET_LINK_LIBRARIES(Triangle3d NrasterDemoUtils)

ADD_EXECUTABLE(Cube4d src/cube4d.cpp)
TARGET_LINK_LIBRARIES(Cube4d NrasterDemoUtils)

if(WIN32)
    # Copy DLL files to output directory
	ADD_CUSTOM_COMMAND(TARGET Triangle3d PRE_LINK
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_BINARY_DIR}/lib/Nraster.dll"         
    $<TARGET_FILE_DIR:Triangle3d>)
endif()
