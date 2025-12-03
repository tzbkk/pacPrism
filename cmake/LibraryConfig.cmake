# LibraryConfig.cmake - Library configurations for pacPrism

# Platform-specific configurations for libraries
function(configure_library_target target_name)
    if(WIN32)
        set_target_properties(${target_name} PROPERTIES
            WINDOWS_EXPORT_ALL_SYMBOLS ON
        )
    endif()

    # Common include directories
    target_include_directories(${target_name} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    )
endfunction()

# Configure network dependencies
function(configure_network_dependencies target_name)
    target_link_libraries(${target_name} PRIVATE Boost::boost)
    if(WIN32)
        target_link_libraries(${target_name} PRIVATE ws2_32 mswsock)
    endif()
endfunction()