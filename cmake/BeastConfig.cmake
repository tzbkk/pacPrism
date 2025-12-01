# BeastConfig.cmake - Auto-generate and use vcpkg for dependency management

set(BEAST_FOUND FALSE)

# If vcpkg toolchain is already provided, use it
if(DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Using provided vcpkg toolchain: ${CMAKE_TOOLCHAIN_FILE}")

    # Find Boost.Beast through existing vcpkg
    find_package(boost-beast CONFIG QUIET)

    if(TARGET Boost::beast)
        set(BEAST_FOUND TRUE)
        message(STATUS "Found Boost.Beast from existing vcpkg: Boost::beast")
    elseif(TARGET boost-beast)
        set(BEAST_FOUND TRUE)
        message(STATUS "Found Boost.Beast from existing vcpkg: boost-beast")
    endif()

    # Also find core Boost components that Beast depends on
    if(BEAST_FOUND)
        find_package(Boost QUIET COMPONENTS system regex thread chrono date_time)
        if(NOT Boost_FOUND)
            message(FATAL_ERROR "Boost components not found. Install: vcpkg install boost-system boost-regex boost-thread boost-chrono boost-date-time")
        endif()
    endif()
endif()

# If no existing vcpkg found, auto-generate vcpkg
if(NOT BEAST_FOUND)
    if(DEFINED CMAKE_TOOLCHAIN_FILE)
        message(STATUS "Existing vcpkg Boost.Beast not found, checking if we need to install packages...")
    else()
        message(STATUS "No vcpkg toolchain specified, auto-generating vcpkg...")
    endif()

    # Auto-generate vcpkg in build directory
    set(VCPKG_ROOT "${CMAKE_BINARY_DIR}/vcpkg")
    set(VCPKG_TOOLCHAIN "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

    # Check if vcpkg already exists in build directory
    if(NOT EXISTS "${VCPKG_ROOT}")
        message(STATUS "Cloning vcpkg into ${VCPKG_ROOT}...")

        # Clone vcpkg
        execute_process(
            COMMAND git clone https://github.com/Microsoft/vcpkg.git "${VCPKG_ROOT}"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            RESULT_VARIABLE vcpkg_clone_result
            OUTPUT_QUIET
            ERROR_QUIET
        )

        if(NOT vcpkg_clone_result EQUAL 0)
            message(FATAL_ERROR "Failed to clone vcpkg repository")
        endif()

        # Bootstrap vcpkg
        if(WIN32)
            message(STATUS "Bootstrapping vcpkg for Windows...")
            execute_process(
                COMMAND "${VCPKG_ROOT}/bootstrap-vcpkg.bat"
                WORKING_DIRECTORY ${VCPKG_ROOT}
                RESULT_VARIABLE vcpkg_bootstrap_result
                OUTPUT_QUIET
                ERROR_QUIET
            )
        else()
            message(STATUS "Bootstrapping vcpkg for Unix...")
            execute_process(
                COMMAND "${VCPKG_ROOT}/bootstrap-vcpkg.sh"
                WORKING_DIRECTORY ${VCPKG_ROOT}
                RESULT_VARIABLE vcpkg_bootstrap_result
                OUTPUT_QUIET
                ERROR_QUIET
            )
        endif()

        if(NOT vcpkg_bootstrap_result EQUAL 0)
            message(FATAL_ERROR "Failed to bootstrap vcpkg")
        endif()

        message(STATUS "vcpkg successfully bootstrapped")
    else()
        message(STATUS "Using existing vcpkg at ${VCPKG_ROOT}")
    endif()

    # Install required vcpkg packages using manifest mode
    message(STATUS "Installing required vcpkg packages using manifest mode...")

    set(vcpkg_triplet "")
    if(WIN32)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(vcpkg_triplet "x64-windows")
        else()
            set(vcpkg_triplet "x86-windows")
        endif()
    else()
        if(APPLE)
            set(vcpkg_triplet "x64-osx")
        else()
            set(vcpkg_triplet "x64-linux")
        endif()
    endif()

    execute_process(
        COMMAND "${VCPKG_ROOT}/vcpkg" install --triplet ${vcpkg_triplet}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE vcpkg_install_result
        OUTPUT_VARIABLE vcpkg_install_output
        ERROR_VARIABLE vcpkg_install_error
    )

    if(NOT vcpkg_install_result EQUAL 0)
        message(STATUS "vcpkg install output: ${vcpkg_install_output}")
        message(STATUS "vcpkg install error: ${vcpkg_install_error}")
        message(FATAL_ERROR "Failed to install vcpkg packages from vcpkg.json")
    endif()

    message(STATUS "vcpkg packages successfully installed from vcpkg.json")

    # Set toolchain for subsequent configuration
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_TOOLCHAIN}" CACHE FILEPATH "Auto-generated vcpkg toolchain" FORCE)

    # Re-run find_package with the generated vcpkg
    find_package(boost-beast CONFIG QUIET)

    if(TARGET Boost::beast)
        set(BEAST_FOUND TRUE)
        message(STATUS "Found Boost.Beast from auto-generated vcpkg: Boost::beast")
    elseif(TARGET boost-beast)
        set(BEAST_FOUND TRUE)
        message(STATUS "Found Boost.Beast from auto-generated vcpkg: boost-beast")
    endif()

    if(BEAST_FOUND)
        find_package(Boost QUIET COMPONENTS system regex thread chrono date_time)
        if(NOT Boost_FOUND)
            message(FATAL_ERROR "Boost components not found in auto-generated vcpkg")
        endif()
    else()
        message(FATAL_ERROR "Failed to find Boost.Beast after auto-generating vcpkg")
    endif()
endif()

# Function to configure Beast with vcpkg (either existing or auto-generated)
function(configure_beast target_name)
    if(TARGET Boost::beast)
        target_link_libraries(${target_name} PRIVATE Boost::beast)
        message(STATUS "Configured ${target_name} with Boost::beast from vcpkg")
    elseif(TARGET boost-beast)
        target_link_libraries(${target_name} PRIVATE boost-beast)
        message(STATUS "Configured ${target_name} with boost-beast from vcpkg")
    else()
        message(FATAL_ERROR "Boost.Beast target not found. vcpkg configuration failed.")
    endif()

    # vcpkg automatically handles Boost component dependencies
    # through the INTERFACE_LINK_LIBRARIES of the Boost::beast target
endfunction()