# pacPrism root CMake configuration

cmake_minimum_required(VERSION 3.14)

project(pacPrism VERSION 0.1.0 LANGUAGES CXX)

# Include configuration scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies from system packages
find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json 3.10.0 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(CXXOPTS REQUIRED cxxopts)

# Version configuration
include(VersionConfig)

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