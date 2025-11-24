# =============================================================================
# pacPrism - Library Components CMake Configuration
# =============================================================================

# -----------------------------------------------------------------------------
# Library Definitions
# -----------------------------------------------------------------------------

# DHT (Distributed Hash Table) Library
# Provides core DHT functionality for node discovery and package location
add_library(node_dht SHARED
    node/dht/dht_operation.cpp
)

# Network Transmission Library
# Handles network communication between nodes using asio
add_library(network_transmission SHARED
    network/transmission/transmission.cpp
)

# -----------------------------------------------------------------------------
# Platform-Specific Configurations
# -----------------------------------------------------------------------------
# Ensure proper import library generation on Windows
if(WIN32)
    set_target_properties(node_dht PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
    set_target_properties(network_transmission PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
endif()

# -----------------------------------------------------------------------------
# Include Directories
# -----------------------------------------------------------------------------
# Set up include directories for libraries
target_include_directories(node_dht PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
)

target_include_directories(network_transmission PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
)

# -----------------------------------------------------------------------------
# Library Dependencies
# -----------------------------------------------------------------------------
# Link asio to network_transmission library for network functionality
target_link_libraries(network_transmission PRIVATE PkgConfig::ASIO)

# Link Windows socket library on Windows for asio functionality
if(WIN32)
    target_link_libraries(network_transmission PRIVATE ws2_32)
endif()