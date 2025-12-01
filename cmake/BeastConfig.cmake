# BeastConfig.cmake - Complete Boost dependency management for pacPrism

# Try to find complete Boost installation first
find_package(Boost 1.70 QUIET COMPONENTS system regex thread chrono date_time)

set(BEAST_FOUND FALSE)

if(Boost_FOUND)
    # Check if beast is available in the Boost installation
    find_path(BOOST_BEAST_INCLUDE_DIR
        NAMES boost/beast.hpp
        PATHS ${Boost_INCLUDE_DIRS}
        NO_DEFAULT_PATH
    )

    if(BOOST_BEAST_INCLUDE_DIR)
        set(BEAST_FOUND TRUE)
        message(STATUS "Found complete Boost with Beast: ${Boost_INCLUDE_DIRS}")

        # Create Beast target if it doesn't exist
        if(NOT TARGET Boost::beast)
            add_library(Boost::beast INTERFACE IMPORTED)
            set_target_properties(Boost::beast PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${BOOST_BEAST_INCLUDE_DIR}"
                INTERFACE_LINK_LIBRARIES "Boost::system;Boost::regex;Boost::thread;Boost::chrono;Boost::date_time"
            )
        endif()
    endif()
endif()

# If complete Boost not found, fetch via FetchContent
if(NOT BEAST_FOUND)
    message(STATUS "Complete Boost not found, fetching via FetchContent...")
    include(FetchContent)

    # Fetch complete Boost distribution
    FetchContent_Declare(
        boost
        GIT_REPOSITORY https://github.com/boostorg/boost.git
        GIT_TAG        boost-1.85.0
    )

    # Initialize Boost submodules for the components we need
    set(BOOST_ENABLE_CMAKE ON CACHE BOOL "Enable CMake support for Boost")
    set(BUILD_TESTING OFF CACHE BOOL "Disable Boost testing")
    set(BOOST_INCLUDE_LIBRARIES "system;regex;thread;chrono;date_time" CACHE STRING "Boost libraries to include")

    FetchContent_MakeAvailable(boost)

    # Create Beast target using the fetched Boost
    add_library(Boost::beast INTERFACE IMPORTED)
    set_target_properties(Boost::beast PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${boost_SOURCE_DIR}"
        INTERFACE_LINK_LIBRARIES "Boost::system;Boost::regex;Boost::thread;Boost::chrono;Boost::date_time"
    )

    set(BEAST_FOUND TRUE)
    message(STATUS "Complete Boost successfully fetched and configured")
endif()

# Function to configure Beast with complete Boost for a target
function(configure_beast target_name)
    if(TARGET Boost::beast)
        target_link_libraries(${target_name} PRIVATE Boost::beast)
        message(STATUS "Configured ${target_name} with Boost::beast (complete Boost)")
    else()
        message(WARNING "Boost.Beast target not found, HTTP functionality may not work properly")
        return()
    endif()

    # All required Boost components are already linked via Boost::beast INTERFACE_LINK_LIBRARIES
    # No need to manually link individual components
endfunction()