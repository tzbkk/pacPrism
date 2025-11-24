# =============================================================================
# pacPrism - Main Application CMake Configuration
# =============================================================================

# -----------------------------------------------------------------------------
# Main Executable
# -----------------------------------------------------------------------------
# Define the main pacPrism executable
add_executable(pacprism main.cpp)

# -----------------------------------------------------------------------------
# Link Libraries
# -----------------------------------------------------------------------------
# Link required libraries to the main executable
target_link_libraries(pacprism PRIVATE node_dht)
target_link_libraries(pacprism PRIVATE network_transmission)