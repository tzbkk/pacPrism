# pacPrism root CMake configuration

cmake_minimum_required(VERSION 3.14)
project(pacPrism VERSION 0.1.0 LANGUAGES CXX)

# Include configuration scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependency
cmake_policy(SET CMP0167 NEW)
find_package(Boost REQUIRED)

# Version configuration
include(VersionConfig)

# Platform-specific configurations
include(PlatformConfig)

# Build configuration
include(BuildConfig)

# Subdirectories
add_subdirectory(lib)
add_subdirectory(src)