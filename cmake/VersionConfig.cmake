# VersionConfig.cmake - Version management for pacPrism

# Extract version information from project()
if(NOT PROJECT_VERSION)
    set(PROJECT_VERSION "0.1.0")
endif()

# Parse version components
string(REPLACE "." ";" VERSION_LIST "${PROJECT_VERSION}")
list(LENGTH VERSION_LIST VERSION_LENGTH)

if(VERSION_LENGTH EQUAL 3)
    list(GET VERSION_LIST 0 PROJECT_VERSION_MAJOR)
    list(GET VERSION_LIST 1 PROJECT_VERSION_MINOR)
    list(GET VERSION_LIST 2 PROJECT_VERSION_PATCH)
elseif(VERSION_LENGTH EQUAL 2)
    list(GET VERSION_LIST 0 PROJECT_VERSION_MAJOR)
    list(GET VERSION_LIST 1 PROJECT_VERSION_MINOR)
    set(PROJECT_VERSION_PATCH "0")
else()
    set(PROJECT_VERSION_MAJOR "0")
    set(PROJECT_VERSION_MINOR "1")
    set(PROJECT_VERSION_PATCH "0")
endif()

# Version strings for different uses
set(PACPRISM_VERSION_FULL "${PROJECT_VERSION}")
set(PACPRISM_VERSION_SHORT "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
set(PACPRISM_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(PACPRISM_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(PACPRISM_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

# Build information
if(NOT PACPRISM_BUILD_TYPE)
    if(CMAKE_BUILD_TYPE)
        set(PACPRISM_BUILD_TYPE "${CMAKE_BUILD_TYPE}")
    else()
        set(PACPRISM_BUILD_TYPE "Debug")
    endif()
endif()

if(NOT PACPRISM_BUILD_DATE)
    string(TIMESTAMP PACPRISM_BUILD_DATE "%Y-%m-%d")
endif()

# Git information (optional) - separate from version number
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE PACPRISM_GIT_COMMIT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --always --dirty
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE PACPRISM_GIT_DESCRIBE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
endif()

# Configure version header
set(VERSION_HEADER_OUTPUT "${CMAKE_BINARY_DIR}/include/pacPrism/version.h")

# Create include directory if it doesn't exist
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/include/pacPrism")

# Generate version.h template
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.h.in"
    "${VERSION_HEADER_OUTPUT}"
    @ONLY
)

# Add include directory to all targets
include_directories("${CMAKE_BINARY_DIR}/include")

# Function to get version info for targets
function(get_version_info target_name)
    target_compile_definitions(${target_name} PRIVATE
        PACPRISM_VERSION_FULL="${PACPRISM_VERSION_FULL}"
        PACPRISM_VERSION_SHORT="${PACPRISM_VERSION_SHORT}"
        PACPRISM_VERSION_MAJOR=${PACPRISM_VERSION_MAJOR}
        PACPRISM_VERSION_MINOR=${PACPRISM_VERSION_MINOR}
        PACPRISM_VERSION_PATCH=${PACPRISM_VERSION_PATCH}
        PACPRISM_BUILD_TYPE="${PACPRISM_BUILD_TYPE}"
        PACPRISM_BUILD_DATE="${PACPRISM_BUILD_DATE}"
    )

    if(PACPRISM_GIT_DESCRIBE)
        target_compile_definitions(${target_name} PRIVATE
            PACPRISM_GIT_DESCRIBE="${PACPRISM_GIT_DESCRIBE}"
        )
    endif()

    if(PACPRISM_GIT_COMMIT)
        target_compile_definitions(${target_name} PRIVATE
            PACPRISM_GIT_COMMIT="${PACPRISM_GIT_COMMIT}"
        )
    endif()
endfunction()

# Print version information
message(STATUS "pacPrism Version Configuration:")
message(STATUS "  Version: ${PACPRISM_VERSION_FULL}")
message(STATUS "  Build Type: ${PACPRISM_BUILD_TYPE}")
message(STATUS "  Build Date: ${PACPRISM_BUILD_DATE}")
if(PACPRISM_GIT_COMMIT)
    message(STATUS "  Git Commit: ${PACPRISM_GIT_COMMIT}")
endif()
if(PACPRISM_GIT_DESCRIBE)
    message(STATUS "  Git Describe: ${PACPRISM_GIT_DESCRIBE}")
endif()
message(STATUS "  Version Header: ${VERSION_HEADER_OUTPUT}")