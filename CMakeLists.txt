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
# Network functionality is mandatory
# Try to find asio package (works with vcpkg and system installs)
find_package(asio QUIET)

if(NOT asio_FOUND)
    # Try pkg-config as fallback
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(ASIO IMPORTED_TARGET asio)
        if(ASIO_FOUND)
            # Create imported target alias for consistency
            add_library(asio::asio INTERFACE IMPORTED)
            set_target_properties(asio::asio PROPERTIES
                INTERFACE_LINK_LIBRARIES PkgConfig::ASIO)
        endif()
    endif()
endif()

if(NOT asio_FOUND AND NOT ASIO_FOUND)
    message(FATAL_ERROR "asio not found. Please install asio development package or use vcpkg.")
endif()

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