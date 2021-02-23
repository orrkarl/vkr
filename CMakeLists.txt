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

# Creating global compiler options pseudo-target
ADD_LIBRARY(vkr-global-compiler-opts INTERFACE)
TARGET_COMPILE_OPTIONS(vkr-global-compiler-opts INTERFACE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror -Wpedantic -Wconversion -Wsign-conversion>
        )

# Adding vkr itself
ADD_SUBDIRECTORY(src)

IF(BUILD_TESTS)
	INCLUDE(FetchContent)

    # Using vk-bootstrap for vulkan boilerplate
    FetchContent_Declare(
            vk-bootstrap
            GIT_REPOSITORY https://github.com/orrkarl/vk-bootstrap.git
            GIT_TAG v0.2.1)
    FetchContent_MakeAvailable(vk-bootstrap)

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.10.0)
    FetchContent_MakeAvailable(googletest)

    ENABLE_TESTING()

    ADD_SUBDIRECTORY(test)
ENDIF()

IF(BUILD_DOCS)
	FIND_PACKAGE(Doxygen REQUIRED)
	ADD_SUBDIRECTORY(docs)
ENDIF()
