add_library(network_dht SHARED
    network/dht/dht_store.cpp
)

target_include_directories(network_dht PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
)