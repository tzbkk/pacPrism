# BeastConfig.cmake - Find and configure Boost.Beast with vcpkg manifest fallback

# Set policy to avoid deprecation warning about FindBoost module
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

set(BEAST_FOUND FALSE)
message(STATUS "Looking for Boost.Beast...")

# Step 1: Try to find system Boost installation first
find_package(Boost QUIET COMPONENTS system regex thread chrono date_time)

if(Boost_FOUND)
    message(STATUS "Found Boost system installation")
    message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")

    # Check if Boost.Beast headers are available
    find_path(BOOST_BEAST_INCLUDE_DIR
        NAMES boost/beast.hpp
        PATHS ${Boost_INCLUDE_DIRS}
        NO_DEFAULT_PATH
    )

    if(BOOST_BEAST_INCLUDE_DIR AND EXISTS "${BOOST_BEAST_INCLUDE_DIR}/boost/beast.hpp")
        message(STATUS "Found Boost.Beast headers: ${BOOST_BEAST_INCLUDE_DIR}")

        # Create Boost::beast target since it's header-only
        if(NOT TARGET Boost::beast)
            add_library(Boost::beast INTERFACE IMPORTED)
            set_target_properties(Boost::beast PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${BOOST_BEAST_INCLUDE_DIR}"
                INTERFACE_LINK_LIBRARIES "Boost::system;Boost::regex;Boost::thread;Boost::chrono;Boost::date_time"
            )
        endif()

        set(BEAST_FOUND TRUE)
        message(STATUS "Configured Boost.Beast from system installation")
    endif()
endif()

# Step 2: If system Boost not found, try vcpkg toolchain if provided
if(NOT BEAST_FOUND AND DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "System Boost not found, trying vcpkg toolchain...")

    # Try CONFIG mode with vcpkg
    find_package(boost-beast CONFIG QUIET)

    if(TARGET boost-beast)
        set(BEAST_FOUND TRUE)
        message(STATUS "Found Boost.Beast from vcpkg: boost-beast")

        # Create Boost::beast alias for consistency
        if(NOT TARGET Boost::beast)
            add_library(Boost::beast INTERFACE IMPORTED)
            target_link_libraries(Boost::beast INTERFACE boost_beast)
        endif()
    elseif(TARGET Boost::beast)
        set(BEAST_FOUND TRUE)
        message(STATUS "Found Boost.Beast from vcpkg: Boost::beast")
    endif()
endif()

# Step 3: If still not found, use vcpkg manifest mode
if(NOT BEAST_FOUND)
    message(STATUS "No existing Boost.Beast found, using vcpkg manifest mode...")

    # Verify vcpkg.json exists
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/vcpkg.json")
        message(FATAL_ERROR "vcpkg.json not found in project root")
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

    # Set up vcpkg toolchain for subsequent find_package calls
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_TOOLCHAIN}" CACHE FILEPATH "vcpkg toolchain for manifest mode" FORCE)

    # Determine vcpkg triplet
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

    # Install vcpkg dependencies from manifest (vcpkg.json)
    message(STATUS "Installing dependencies from vcpkg.json (manifest mode)...")
    message(STATUS "Running: ${VCPKG_ROOT}/vcpkg install --triplet ${vcpkg_triplet}")
    execute_process(
        COMMAND "${VCPKG_ROOT}/vcpkg" install --triplet ${vcpkg_triplet}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE vcpkg_install_result
        OUTPUT_VARIABLE vcpkg_install_output
        ERROR_VARIABLE vcpkg_install_error
        ECHO_OUTPUT_VARIABLE
        ECHO_ERROR_VARIABLE
    )

    if(NOT vcpkg_install_result EQUAL 0)
        message(STATUS "vcpkg install result: ${vcpkg_install_result}")
        message(STATUS "vcpkg install output: ${vcpkg_install_output}")
        message(STATUS "vcpkg install error: ${vcpkg_install_error}")
        message(FATAL_ERROR "Failed to install vcpkg dependencies from vcpkg.json")
    endif()

    message(STATUS "vcpkg dependencies successfully installed from manifest")

    # 设置 vcpkg 安装路径
    set(VCPKG_INSTALLED_DIR "${VCPKG_ROOT}/installed/${vcpkg_triplet}")
    list(APPEND CMAKE_PREFIX_PATH "${VCPKG_INSTALLED_DIR}")
    message(STATUS "Added to CMAKE_PREFIX_PATH: ${VCPKG_INSTALLED_DIR}")

# Find boost-beast from the vcpkg installation
find_package(boost_beast CONFIG QUIET)


    if(TARGET boost_beast)
        set(BEAST_FOUND TRUE)
        message(STATUS "Found boost_beast target from vcpkg manifest")

        # Create Boost::beast alias for consistency
        if(NOT TARGET Boost::beast)
            add_library(Boost::beast INTERFACE IMPORTED)
            target_link_libraries(Boost::beast INTERFACE boost_beast)
        endif()
    elseif(TARGET Boost::beast)
        set(BEAST_FOUND TRUE)
        message(STATUS "Found Boost::beast target from vcpkg manifest")
    else()
        # Try the alternative naming convention
        find_package(boost_beast CONFIG QUIET)

        if(TARGET boost_beast)
            set(BEAST_FOUND TRUE)
            message(STATUS "Found boost_beast target from vcpkg manifest")

            # Create Boost::beast alias for consistency
            if(NOT TARGET Boost::beast)
                add_library(Boost::beast INTERFACE IMPORTED)
                target_link_libraries(Boost::beast INTERFACE boost_beast)
            endif()
        else()
            # Fallback: find headers manually and create target
            set(VCPKG_INSTALLED_DIR "${VCPKG_ROOT}/installed/${vcpkg_triplet}")
            find_path(BOOST_BEAST_INCLUDE_DIR
                NAMES boost/beast.hpp
                PATHS "${VCPKG_INSTALLED_DIR}/include"
                NO_DEFAULT_PATH
            )

            if(BOOST_BEAST_INCLUDE_DIR)
                message(STATUS "Found Boost.Beast headers from vcpkg: ${BOOST_BEAST_INCLUDE_DIR}")

                # Create Boost::beast target
                if(NOT TARGET Boost::beast)
                    add_library(Boost::beast INTERFACE IMPORTED)
                    set_target_properties(Boost::beast PROPERTIES
                        INTERFACE_INCLUDE_DIRECTORIES "${BOOST_BEAST_INCLUDE_DIR}"
                    )
                endif()

                set(BEAST_FOUND TRUE)
                message(STATUS "Configured Boost.Beast from vcpkg manifest headers")
            else()
                message(FATAL_ERROR "Boost.Beast not found after vcpkg manifest installation")
            endif()
        endif()
    endif()
endif()

# Final validation
if(NOT BEAST_FOUND)
    message(FATAL_ERROR "Boost.Beast could not be found or installed")
endif()

# Function to configure Beast for targets
function(configure_beast target_name)
    if(TARGET Boost::beast)
        target_link_libraries(${target_name} PRIVATE Boost::beast)
        message(STATUS "Configured ${target_name} with Boost.Beast")
    else()
        message(FATAL_ERROR "Boost.Beast target not available")
    endif()
endfunction()