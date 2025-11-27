# pacPrism root CMake configuration

cmake_minimum_required(VERSION 3.14)
project(pacPrism VERSION 0.1.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include configuration scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/scripts")

# Platform-specific configurations
include(PlatformConfig)

# External dependencies
include(AsioConfig)
include(BeastConfig)

# Build configuration
include(BuildConfig)

# Subdirectories
add_subdirectory(lib)
add_subdirectory(src)