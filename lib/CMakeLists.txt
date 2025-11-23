add_library(node_dht SHARED
    node/dht/dht_store.cpp
)

target_include_directories(node_dht PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
)