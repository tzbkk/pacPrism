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

    # Find or fetch basic Boost libraries first
    find_package(Boost 1.70 QUIET COMPONENTS system regex)
    if(NOT Boost_FOUND)
        message(STATUS "Boost not found, fetching core components via FetchContent...")

        # Fetch essential boost components that beast depends on
        FetchContent_Declare(
            boost_assert
            GIT_REPOSITORY https://github.com/boostorg/assert.git
            GIT_TAG        boost-1.85.0
        )
        FetchContent_Declare(
            boost_align
            GIT_REPOSITORY https://github.com/boostorg/align.git
            GIT_TAG        boost-1.85.0
        )
        FetchContent_Declare(
            boost_system
            GIT_REPOSITORY https://github.com/boostorg/system.git
            GIT_TAG        boost-1.85.0
        )
        FetchContent_Declare(
            boost_regex
            GIT_REPOSITORY https://github.com/boostorg/regex.git
            GIT_TAG        boost-1.85.0
        )

        FetchContent_MakeAvailable(boost_assert)
        FetchContent_MakeAvailable(boost_align)
        FetchContent_MakeAvailable(boost_system)
        FetchContent_MakeAvailable(boost_regex)
    endif()

    # Use standalone Asio (not Boost.Asio) to avoid complex Boost dependencies
    if(NOT TARGET asio)
        FetchContent_Declare(
            asio
            GIT_REPOSITORY https://github.com/chrismackenzie/asio.git
            GIT_TAG        asio-1-28-2
        )
        FetchContent_MakeAvailable(asio)
    endif()

    # Fetch Boost.Beast
    FetchContent_Declare(
        boost-beast
        GIT_REPOSITORY https://github.com/boostorg/beast.git
        GIT_TAG        boost-1.85.0
    )

    # Set beast as header-only to avoid complex link dependencies
    set(BOOST_BEAST_HEADER_ONLY TRUE CACHE BOOL "Build Boost.Beast as header-only")
    # Disable optional dependencies that cause issues
    set(BOOST_BEAST_DISABLE_BOOST_URL TRUE CACHE BOOL "Disable Boost.URL dependency")

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

    # Link with standalone Asio (preferred over Boost.Asio to avoid dependency issues)
    if(TARGET asio)
        target_link_libraries(${target_name} PRIVATE asio)
    elseif(TARGET Boost::asio)
        target_link_libraries(${target_name} PRIVATE Boost::asio)
    endif()

    # Link with Boost core components if available
    if(TARGET Boost::system)
        target_link_libraries(${target_name} PRIVATE Boost::system)
    elseif(TARGET boost_system)
        target_link_libraries(${target_name} PRIVATE boost_system)
    endif()

    if(TARGET Boost::regex)
        target_link_libraries(${target_name} PRIVATE Boost::regex)
    elseif(TARGET boost_regex)
        target_link_libraries(${target_name} PRIVATE boost_regex)
    endif()

    # Link other essential Boost components for Beast compatibility
    if(TARGET boost_assert)
        target_link_libraries(${target_name} PRIVATE boost_assert)
    endif()

    if(TARGET boost_align)
        target_link_libraries(${target_name} PRIVATE boost_align)
    endif()
endfunction()