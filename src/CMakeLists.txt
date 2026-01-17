# pacPrism library components CMake configuration

# Include configuration scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
include(LibraryConfig)

# Library definitions
add_library(node_dht SHARED
    node/dht/dht_operation.cpp
)

add_library(node_validator SHARED
    node/validator/validator.cpp
)

add_library(console_parser SHARED
    console/parser/parser.cpp
)

add_library(console_banner SHARED
    console/banner/banner.cpp
)

add_library(console_io SHARED
    console/io/io.cpp
)

add_library(network_transmission SHARED
    network/transmission/transmission.cpp
)

add_library(network_router SHARED
    network/router/router.cpp
)

# Configure library targets
configure_library_target(node_dht)
configure_library_target(node_validator)
configure_library_target(console_parser)
configure_library_target(console_banner)
configure_library_target(console_io)
configure_library_target(network_transmission)
configure_library_target(network_router)

# Add cxxopts include directory for console_parser
target_include_directories(console_parser PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../cxxopts/include
)

# Apply version information to library targets
get_version_info(node_dht)
get_version_info(node_validator)
get_version_info(console_parser)
get_version_info(console_banner)
get_version_info(console_io)
get_version_info(network_transmission)
get_version_info(network_router)

# Configure network-specific dependencies
configure_network_dependencies(node_validator)
configure_network_dependencies(console_io)
configure_network_dependencies(network_transmission)
configure_network_dependencies(network_router)

# Link bcrypt library for SHA256 support (Windows only)
if(WIN32)
    target_link_libraries(node_validator PRIVATE bcrypt)
endif()

# Link libraries
target_link_libraries(network_router PRIVATE node_validator console_io)
target_link_libraries(network_transmission PRIVATE network_router)

# Main executable
add_executable(pacprism main.cpp)

# Link libraries
target_link_libraries(pacprism PRIVATE node_dht)
target_link_libraries(pacprism PRIVATE node_validator)
target_link_libraries(pacprism PRIVATE console_parser)
target_link_libraries(pacprism PRIVATE console_banner)
target_link_libraries(pacprism PRIVATE console_io)
target_link_libraries(pacprism PRIVATE network_transmission)
target_link_libraries(pacprism PRIVATE network_router)

# Apply version information to executable
get_version_info(pacprism)

# Configure network dependencies for executable
configure_network_dependencies(pacprism)

# Create config directory in root build folder
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/config)

# Copy configuration files to build/config directory
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/../config/pacprism.conf
    ${CMAKE_BINARY_DIR}/config/pacprism.conf
    COPYONLY
)

# Set output directory for config files
set(CONFIG_OUTPUT_DIR ${CMAKE_BINARY_DIR}/config)
message(STATUS "Config files will be copied to: ${CONFIG_OUTPUT_DIR}")