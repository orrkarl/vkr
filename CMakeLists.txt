CMAKE_MINIMUM_REQUIRED(VERSION 3.17)

project(VulkanRasterizer VERSION 0.7.0 LANGUAGES CXX)

IF(NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE Debug)
ENDIF()

# Default options:
OPTION(BUILD_TESTS "build vkr tests" ON)
OPTION(BUILD_DOCS  "generate vkr documentation (requires Doxygen)" ON)

# General settings
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

# Adding required packages for main project
FIND_PACKAGE(Vulkan REQUIRED)

# Including required cmake utilities
include(cmake/glslc.cmake)

# Adding vkr itself
ADD_SUBDIRECTORY(src)

IF(BUILD_TESTS)
	INCLUDE(FetchContent)

    # Tests based on Catch2 suite
	FetchContent_Declare(
			catch2
			GIT_REPOSITORY https://github.com/catchorg/Catch2.git
			GIT_TAG v2.13.3)
    FetchContent_MakeAvailable(catch2)

    # Using vk-bootstrap for vulkan boilerplate
    FetchContent_Declare(
            vk-bootstrap
            GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap.git
            GIT_TAG v0.2)
    FetchContent_MakeAvailable(vk-bootstrap)

    INCLUDE(${catch2_SOURCE_DIR}/contrib/Catch.cmake)

    ENABLE_TESTING()

    ADD_SUBDIRECTORY(test)
ENDIF()

IF(BUILD_DOCS)
	FIND_PACKAGE(Doxygen REQUIRED)
	ADD_SUBDIRECTORY(docs)
ENDIF()
