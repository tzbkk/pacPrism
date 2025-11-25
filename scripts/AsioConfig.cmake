# AsioConfig.cmake - asio dependency management for pacPrism

# Function to download and build asio if not found
function(fetch_asio_dependency)
    include(FetchContent)

    find_package(asio QUIET)

    if(NOT asio_FOUND)
        message(STATUS "asio not found, fetching asio dependency automatically...")

        FetchContent_Declare(
            asio
            GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
            GIT_TAG        asio-1-34-2
        )

        FetchContent_MakeAvailable(asio)

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

# Find asio package
find_package(asio QUIET)

if(NOT asio_FOUND)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(ASIO IMPORTED_TARGET asio)
        if(ASIO_FOUND)
            if(NOT TARGET asio::asio)
                add_library(asio::asio INTERFACE IMPORTED)
                set_target_properties(asio::asio PROPERTIES
                    INTERFACE_LINK_LIBRARIES PkgConfig::ASIO)
            endif()
        endif()
    endif()
endif()

# Fetch asio if not found
if(NOT asio_FOUND AND NOT ASIO_FOUND)
    fetch_asio_dependency()
endif()

# Check asio availability
if(NOT TARGET asio::asio AND NOT asio_FOUND AND NOT ASIO_FOUND)
    message(FATAL_ERROR "Failed to configure asio dependency. Please ensure asio is available or check network connectivity for automatic fetching.")
endif()