add_library(node_dht SHARED
    node/dht/dht_operation.cpp
)

# Ensure proper import library generation on Windows
if(WIN32)
    set_target_properties(node_dht PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
endif()

target_include_directories(node_dht PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
)