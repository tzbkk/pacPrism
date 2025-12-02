# pacPrism library components CMake configuration

# Include configuration scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
include(LibraryConfig)

# Library definitions
add_library(node_dht SHARED
    node/dht/dht_operation.cpp
)

add_library(network_transmission SHARED
    network/transmission/transmission.cpp
)

# Configure library targets
configure_library_target(node_dht)
configure_library_target(network_transmission)

# Apply version information to library targets
get_version_info(node_dht)
get_version_info(network_transmission)

# Configure network-specific dependencies
configure_network_dependencies(network_transmission)