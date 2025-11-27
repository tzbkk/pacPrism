# BeastConfig.cmake - Boost.Beast dependency management for pacPrism

# Try to find Boost.Beast through vcpkg first
find_package(boost_beast CONFIG QUIET)

set(BEAST_FOUND FALSE)

if(TARGET Boost::beast)
    set(BEAST_FOUND TRUE)
    message(STATUS "Found Boost.Beast from vcpkg: Boost::beast")
elseif(TARGET boost-beast)
    set(BEAST_FOUND TRUE)
    message(STATUS "Found Boost.Beast: boost-beast")
else()
    # Try to find Boost.Beast in system Boost installation
    find_package(Boost 1.70 QUIET COMPONENTS system regex)
    if(Boost_FOUND)
        # Check if beast is available in boost installation
        find_path(BOOST_BEAST_INCLUDE_DIR
            NAMES boost/beast.hpp
            PATHS ${Boost_INCLUDE_DIRS}
            NO_DEFAULT_PATH
        )

        if(BOOST_BEAST_INCLUDE_DIR)
            set(BEAST_FOUND TRUE)
            message(STATUS "Found Boost.Beast in system Boost: ${BOOST_BEAST_INCLUDE_DIR}")
            # Create a target if it doesn't exist
            if(NOT TARGET Boost::beast)
                add_library(Boost::beast INTERFACE IMPORTED)
                set_target_properties(Boost::beast PROPERTIES
                    INTERFACE_INCLUDE_DIRECTORIES "${BOOST_BEAST_INCLUDE_DIR}"
                )
            endif()
        endif()
    endif()
endif()

# If still not found, use FetchContent as fallback
if(NOT BEAST_FOUND)
    message(STATUS "Boost.Beast not found, fetching via FetchContent...")
    include(FetchContent)

    FetchContent_Declare(
        boost-beast
        GIT_REPOSITORY https://github.com/boostorg/beast.git
        GIT_TAG        boost-1.85.0
    )

    # Set beast as header-only
    set(BOOST_BEAST_HEADER_ONLY TRUE CACHE BOOL "Build Boost.Beast as header-only")

    FetchContent_MakeAvailable(boost-beast)

    # Create the target if not automatically created
    if(TARGET boost-beast AND NOT TARGET Boost::beast)
        add_library(Boost::beast ALIAS boost-beast)
    endif()

    set(BEAST_FOUND TRUE)
    message(STATUS "Boost.Beast successfully fetched and configured")
endif()

# Function to configure Beast for a target
function(configure_beast target_name)
    # Configure Beast dependencies

    if(TARGET Boost::beast)
        target_link_libraries(${target_name} PRIVATE Boost::beast)
        message(STATUS "Configured ${target_name} with Boost::beast")
    elseif(TARGET boost-beast)
        target_link_libraries(${target_name} PRIVATE boost-beast)
        message(STATUS "Configured ${target_name} with boost-beast")
    else()
        message(WARNING "No Beast target found, HTTP functionality may not work properly")
        return()
    endif()

    # Beast also needs boost system and other core components
    if(TARGET Boost::system)
        target_link_libraries(${target_name} PRIVATE Boost::system)
    endif()

    if(TARGET Boost::regex)
        target_link_libraries(${target_name} PRIVATE Boost::regex)
    endif()

    # Asio integration - Beast works well with asio
    if(TARGET asio::asio)
        target_link_libraries(${target_name} PRIVATE asio::asio)
    endif()
endfunction()