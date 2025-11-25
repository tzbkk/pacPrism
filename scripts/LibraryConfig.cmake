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
    if(TARGET asio::asio)
        target_link_libraries(${target_name} PRIVATE asio::asio)
    else()
        target_link_libraries(${target_name} PRIVATE ${ASIO_LIBRARIES})
        target_include_directories(${target_name} PRIVATE ${ASIO_INCLUDE_DIRS})
    endif()

    # Windows socket library
    if(WIN32)
        target_link_libraries(${target_name} PRIVATE ws2_32)
    endif()
endfunction()