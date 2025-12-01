# BeastConfig.cmake - Hybrid dependency management: vcpkg for dev, FetchContent for CI

# Option to force FetchContent mode (useful for CI)
option(PACPRISM_USE_FETCHCONTENT "Force FetchContent instead of vcpkg" OFF)

set(BEAST_FOUND FALSE)

# Prefer vcpkg unless FetchContent is explicitly requested
if(NOT PACPRISM_USE_FETCHCONTENT AND DEFINED CMAKE_TOOLCHAIN_FILE)
    # vcpkg mode - for development/production builds
    message(STATUS "Using vcpkg dependency management")

    # Find Boost.Beast through vcpkg
    find_package(boost-beast CONFIG QUIET)

    if(TARGET Boost::beast)
        set(BEAST_FOUND TRUE)
        message(STATUS "Found Boost.Beast from vcpkg: Boost::beast")
    elseif(TARGET boost-beast)
        set(BEAST_FOUND TRUE)
        message(STATUS "Found Boost.Beast from vcpkg: boost-beast")
    endif()

    # Also find core Boost components that Beast depends on
    if(BEAST_FOUND)
        find_package(Boost QUIET COMPONENTS system regex thread chrono date_time)
        if(NOT Boost_FOUND)
            message(FATAL_ERROR "Boost components not found. Install: vcpkg install boost-system boost-regex boost-thread boost-chrono boost-date-time")
        endif()
    endif()
endif()

# If vcpkg failed or FetchContent is requested, use FetchContent
if(NOT BEAST_FOUND)
    if(PACPRISM_USE_FETCHCONTENT)
        message(STATUS "Using FetchContent dependency management (CI mode)")
    elseif(DEFINED CMAKE_TOOLCHAIN_FILE)
        message(STATUS "vcpkg Boost.Beast not found, falling back to FetchContent")
    else()
        message(STATUS "No vcpkg toolchain specified, using FetchContent")
    endif()

    include(FetchContent)

    # Fetch complete Boost distribution
    FetchContent_Declare(
        boost
        GIT_REPOSITORY https://github.com/boostorg/boost.git
        GIT_TAG        boost-1.85.0
    )

    # Configure Boost for FetchContent
    set(BOOST_ENABLE_CMAKE ON CACHE BOOL "Enable CMake support for Boost")
    set(BUILD_TESTING OFF CACHE BOOL "Disable Boost testing")
    set(BOOST_INCLUDE_LIBRARIES "system;regex;thread;chrono;date_time" CACHE STRING "Boost libraries to include")

    FetchContent_MakeAvailable(boost)

    # Create Boost::beast target using fetched Boost
    if(NOT TARGET Boost::beast)
        add_library(Boost::beast INTERFACE IMPORTED)
        set_target_properties(Boost::beast PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${boost_SOURCE_DIR}"
            INTERFACE_LINK_LIBRARIES "Boost::system;Boost::regex;Boost::thread;Boost::chrono;Boost::date_time"
        )
    endif()

    set(BEAST_FOUND TRUE)
    message(STATUS "Complete Boost successfully fetched and configured via FetchContent")
endif()

# Function to configure Beast with either vcpkg or FetchContent
function(configure_beast target_name)
    if(TARGET Boost::beast)
        target_link_libraries(${target_name} PRIVATE Boost::beast)
        if(DEFINED CMAKE_TOOLCHAIN_FILE AND NOT PACPRISM_USE_FETCHCONTENT)
            message(STATUS "Configured ${target_name} with Boost::beast from vcpkg")
        else()
            message(STATUS "Configured ${target_name} with Boost::beast from FetchContent")
        endif()
    elseif(TARGET boost-beast)
        target_link_libraries(${target_name} PRIVATE boost-beast)
        if(DEFINED CMAKE_TOOLCHAIN_FILE AND NOT PACPRISM_USE_FETCHCONTENT)
            message(STATUS "Configured ${target_name} with boost-beast from vcpkg")
        else()
            message(STATUS "Configured ${target_name} with boost-beast from FetchContent")
        endif()
    else()
        message(FATAL_ERROR "Boost.Beast target not found. Dependency configuration failed.")
    endif()

    # Both vcpkg and FetchContent modes automatically handle Boost component dependencies
    # through the INTERFACE_LINK_LIBRARIES of the Boost::beast target
endfunction()