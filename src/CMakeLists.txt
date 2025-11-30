# pacPrism main application CMake configuration

# Include configuration scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../scripts")
include(LibraryConfig)

# Main executable
add_executable(pacprism main.cpp)

# Link libraries
target_link_libraries(pacprism PRIVATE node_dht)
target_link_libraries(pacprism PRIVATE network_transmission)

# Apply version information to executable
get_version_info(pacprism)

# Configure network dependencies for executable
configure_network_dependencies(pacprism)

# Configure Boost.Beast for executable (since it includes transmission headers)
configure_beast(pacprism)