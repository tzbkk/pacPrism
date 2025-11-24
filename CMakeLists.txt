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
# Function to download and build asio if not found
function(fetch_asio_dependency)
    include(FetchContent)

    # Check if asio is already available
    find_package(asio QUIET)

    if(NOT asio_FOUND)
        message(STATUS "asio not found, fetching asio dependency automatically...")

        # Fetch asio from GitHub
        FetchContent_Declare(
            asio
            GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
            GIT_TAG        asio-1-34-2  # Use a specific release tag
        )

        FetchContent_MakeAvailable(asio)

        # Create an alias target for consistency
        if(NOT TARGET asio::asio)
            add_library(asio::asio INTERFACE IMPORTED)
            set_target_properties(asio::asio PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${asio_SOURCE_DIR}/asio/include"
            )
        endif()

        message(STATUS "asio successfully fetched and configured")
    else()
        message(STATUS "asio found: ${asio_VERSION}")
    endif()
endfunction()

# Try to find asio package (works with vcpkg and system installs)
find_package(asio QUIET)

if(NOT asio_FOUND)
    # Try pkg-config as fallback
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(ASIO IMPORTED_TARGET asio)
        if(ASIO_FOUND)
            # Create imported target alias for consistency
            if(NOT TARGET asio::asio)
                add_library(asio::asio INTERFACE IMPORTED)
                set_target_properties(asio::asio PROPERTIES
                    INTERFACE_LINK_LIBRARIES PkgConfig::ASIO)
            endif()
        endif()
    endif()
endif()

# If asio is still not found, fetch it automatically
if(NOT asio_FOUND AND NOT ASIO_FOUND)
    fetch_asio_dependency()
endif()

# Final check
if(NOT TARGET asio::asio AND NOT asio_FOUND AND NOT ASIO_FOUND)
    message(FATAL_ERROR "Failed to configure asio dependency. Please ensure asio is available or check network connectivity for automatic fetching.")
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