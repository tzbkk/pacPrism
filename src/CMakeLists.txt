# pacPrism main application CMake configuration

# Main executable
add_executable(pacprism main.cpp)

# Link libraries
target_link_libraries(pacprism PRIVATE node_dht)
target_link_libraries(pacprism PRIVATE network_transmission)