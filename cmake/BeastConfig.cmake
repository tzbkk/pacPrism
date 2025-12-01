# BeastConfig.cmake - vcpkg dependency management for pacPrism

# vcpkg integration - vcpkg must be pre-installed on build machines
if(DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Using vcpkg toolchain: ${CMAKE_TOOLCHAIN_FILE}")
else()
    message(FATAL_ERROR "vcpkg toolchain not specified. Please install vcpkg and set CMAKE_TOOLCHAIN_FILE to vcpkg.cmake")
endif()

# Required packages for vcpkg
set(VCPKG_TARGET_TRIPLET ${VCPKG_TARGET_TRIPLET} CACHE STRING "Vcpkg target triplet")
set(VCPKG_MANIFEST_MODE OFF CACHE BOOL "Disable vcpkg manifest mode, use explicit find_package")

# Find Boost.Beast through vcpkg
find_package(boost-beast CONFIG REQUIRED)

set(BEAST_FOUND FALSE)

if(TARGET Boost::beast)
    set(BEAST_FOUND TRUE)
    message(STATUS "Found Boost.Beast from vcpkg: Boost::beast")
elseif(TARGET boost-beast)
    set(BEAST_FOUND TRUE)
    message(STATUS "Found Boost.Beast from vcpkg: boost-beast")
else()
    message(FATAL_ERROR "Boost.Beast not found. Please install with vcpkg: vcpkg install boost-beast")
endif()

# Also find core Boost components that Beast depends on
find_package(Boost REQUIRED COMPONENTS system regex thread chrono date_time)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Core Boost components not found. Please install with vcpkg: vcpkg install boost-system boost-regex boost-thread boost-chrono boost-date-time")
endif()

# Function to configure Beast with vcpkg for a target
function(configure_beast target_name)
    if(TARGET Boost::beast)
        target_link_libraries(${target_name} PRIVATE Boost::beast)
        message(STATUS "Configured ${target_name} with Boost::beast from vcpkg")
    elseif(TARGET boost-beast)
        target_link_libraries(${target_name} PRIVATE boost-beast)
        message(STATUS "Configured ${target_name} with boost-beast from vcpkg")
    else()
        message(FATAL_ERROR "Boost.Beast target not found. vcpkg installation may be incomplete.")
    endif()

    # vcpkg automatically handles Boost component dependencies
    # No need to manually link individual Boost components
endfunction()