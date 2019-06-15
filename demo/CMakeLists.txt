CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(GLEW REQUIRED)
FIND_PACKAGE(glfw3 3.3 REQUIRED)

INCLUDE_DIRECTORIES(
	PUBLIC inc
	PUBLIC ${NRASTER_ALL_INCLUDES}
	PUBLIC ${OpenCL_INCLUDE_DIRS}
	PUBLIC ${OPENGL_INCLUDE_DIRS}
	PUBLIC ${GLEW_INCLUDE_DIRS}
	PUBLIC ${GLFW_INCLUDE_DIRS})

ADD_LIBRARY(NrasterDemoUtils src/utils.cpp src/linalg.cpp)

TARGET_LINK_LIBRARIES(NrasterDemoUtils 
	PUBLIC Nraster
	PUBLIC OpenCL::OpenCL
	PUBLIC opengl32
	PUBLIC glfw
	PUBLIC libglew_shared)

ADD_EXECUTABLE(Triangle3d src/triangle3d.cpp)
TARGET_LINK_LIBRARIES(Triangle3d NrasterDemoUtils)

ADD_EXECUTABLE(Cube4d src/cube4d.cpp)
TARGET_LINK_LIBRARIES(Cube4d NrasterDemoUtils)


if(WIN32)
    # Copy DLL files to output directory
    add_custom_command(TARGET Triangle3d POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_BINARY_DIR}/lib/Nraster.dll"             
    $<TARGET_FILE_DIR:Triangle3d>)
endif()
