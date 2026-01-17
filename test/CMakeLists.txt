# pacPrism Test Suite CMake Configuration

cmake_minimum_required(VERSION 3.14)

# Include parent configuration
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
include(LibraryConfig)

# Test executable
add_executable(pacprism_tests
    main.cpp
    node/validator/test_validator.cpp
    node/dht/test_dht.cpp
    node/package/test_parser.cpp
    console/parser/test_parser.cpp
    console/banner/test_banner.cpp
    console/io/test_io.cpp
    network/transmission/test_transmission.cpp
    network/router/test_router.cpp
)

# Include directories
target_include_directories(pacprism_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../cxxopts/include
    ${CMAKE_BINARY_DIR}/include
)

# Link libraries
target_link_libraries(pacprism_tests PRIVATE
    node_validator
    node_dht
    package_parser
    console_parser
    console_banner
    console_io
    network_transmission
    network_router
)

# Windows-specific linking
if(WIN32)
    target_link_libraries(pacprism_tests PRIVATE
        bcrypt
        ws2_32
        wsock32
    )
endif()

# Configure network dependencies
configure_network_dependencies(pacprism_tests)

# Apply version information
get_version_info(pacprism_tests)

# Enable testing
enable_testing()
add_test(NAME pacprism_test_suite COMMAND pacprism_tests)

# Custom target to run tests
add_custom_target(run_tests
    COMMAND pacprism_tests
    DEPENDS pacprism_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running pacPrism test suite..."
)
