# =============================================================================
# pacPrism - Root CMake Configuration
# =============================================================================

cmake_minimum_required(VERSION 3.14)
project(pacPrism VERSION 0.1.0 LANGUAGES CXX)

# -----------------------------------------------------------------------------
# C++ Standard Requirements
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# Platform-Specific Definitions
# -----------------------------------------------------------------------------
# Define WIN32_WINNT for asio on Windows to avoid compilation warnings
if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0601)
endif()

# -----------------------------------------------------------------------------
# External Dependencies
# -----------------------------------------------------------------------------
# Find required packages for the project
find_package(PkgConfig REQUIRED)
pkg_check_modules(ASIO REQUIRED IMPORTED_TARGET asio)

# -----------------------------------------------------------------------------
# Build Configuration
# -----------------------------------------------------------------------------
# Set output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# -----------------------------------------------------------------------------
# Subdirectories
# -----------------------------------------------------------------------------
# Add library and source subdirectories
add_subdirectory(lib)
add_subdirectory(src)