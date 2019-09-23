CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

project(Nraster VERSION 0.6.1 LANGUAGES CXX)

IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE Debug)
ENDIF()

# Default options:
OPTION(BUILD_TESTS "builds the test directory and gtest" ON)
OPTION(BUILD_DEMOS "builds the demo directory" ON)
OPTION(BUILD_DOCS  "builds the Nraster documentation (requires Doxygen)" ON)

# General settings
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)


# Platform specific settings:
IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	ADD_COMPILE_OPTIONS(-Wno-ignored-attributes)
ENDIF()

IF(WIN32)
	SET(gtest_force_shared_crt ON)
ENDIF(WIN32)


# Must-have packeges
FIND_PACKAGE(OpenCL REQUIRED)


# Adding Nraster itself
ADD_SUBDIRECTORY(project)
SET(NRASTER_ALL_INCLUDES ${PROJECT_SOURCE_DIR}/project/src)
SET(NRASTER_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/project/include)


# Conditional packeges and targets
IF(BUILD_DEMOS)
	ADD_SUBDIRECTORY(dependencies/glew EXCLUDE_FROM_ALL)
	SET(glew_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glew/include")
	ADD_LIBRARY(glew ALIAS libglew_shared)

	SET(GLFW_BUILD_EXAMPLES OFF)
	SET(GLFW_BUILD_TESTS OFF)
	SET(GLFW_BUILD_DOCS OFF)
	ADD_SUBDIRECTORY(dependencies/glfw EXCLUDE_FROM_ALL)
	SET(glfw_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glfw/include")
	
	FIND_PACKAGE(OpenGL REQUIRED)
	SET(OpenGL_INCLUDE_DIRS "${OPENGL_INCLUDE_DIRS}")
	
	ADD_SUBDIRECTORY(demo)
ENDIF()

IF(BUILD_TESTS)	
	ADD_SUBDIRECTORY(dependencies/googletest EXCLUDE_FROM_ALL)
	SET(gtest_INCLUDE_DIRS "${gtest_SOURCE_DIR}/include ${gmock_SOURCE_DIR}/include")

	ADD_SUBDIRECTORY(test)
	ENABLE_TESTING()
	ADD_TEST(NAME AllTests COMMAND NrasterTest) # run the executable itself for a better log as it uses GTest
ENDIF()

IF(BUILD_DOCS)
	FIND_PACKAGE(Doxygen REQUIRED)
	ADD_SUBDIRECTORY(docs)
ENDIF()

UNSET(NRASTER_ALL_INCLUDES)
