# PlatformConfig.cmake - Platform-specific configurations for pacPrism

# Platform-specific definitions
if(WIN32)
    add_compile_definitions(_WIN32_WINNT=0x0601)
endif()