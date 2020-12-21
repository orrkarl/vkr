CMAKE_MINIMUM_REQUIRED(VERSION 3.17)

project(Nraster VERSION 0.7.0 LANGUAGES CXX)

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

include(cmake/glslc.cmake)

# Adding Nraster itself
ADD_SUBDIRECTORY(src)
SET(NRASTER_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/project/include)

IF(BUILD_TESTS)
	INCLUDE(FetchContent)

	FetchContent_Declare(
			catch2
			GIT_REPOSITORY https://github.com/catchorg/Catch2.git
			GIT_TAG v2.13.3)
    FetchContent_MakeAvailable(catch2)

	INCLUDE(${catch2_SOURCE_DIR}/contrib/Catch.cmake)

	ENABLE_TESTING()

	ADD_SUBDIRECTORY(test)
ENDIF()

IF(BUILD_DOCS)
	FIND_PACKAGE(Doxygen REQUIRED)
	ADD_SUBDIRECTORY(docs)
ENDIF()
