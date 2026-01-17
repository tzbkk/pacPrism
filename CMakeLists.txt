# pacPrism root CMake configuration

cmake_minimum_required(VERSION 3.14)

# Policy must be set BEFORE project() call
# CMP0167: Use Boost Config module instead of FindBoost
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

project(pacPrism VERSION 0.1.0 LANGUAGES CXX)

# Include configuration scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependency
find_package(Boost REQUIRED)

# Version configuration
include(VersionConfig)

# Platform-specific configurations
include(PlatformConfig)

# Build configuration
include(BuildConfig)

# Subdirectories
add_subdirectory(src)

# Tests (optional, can be enabled with BUILD_TESTING=ON)
option(BUILD_TESTING "Build test suite" OFF)
if(BUILD_TESTING)
    add_subdirectory(test)
    message(STATUS "Test suite enabled")
endif()