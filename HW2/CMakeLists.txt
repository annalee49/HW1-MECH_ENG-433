cmake_minimum_required(VERSION 3.13)

# Path to Pico SDK
set(PICO_SDK_PATH "/Users/annalee/Documents/GitHub/pico-sdk")

# Include SDK
include(pico_sdk_import.cmake)

# Project
project(HW2_project C CXX ASM)

# Initialize Pico SDK
pico_sdk_init()

# Add source files
add_executable(HW2
    main.c
)

# Link Pico SDK libraries including the hardware modules you use
target_link_libraries(HW2
    pico_stdlib
    hardware_pwm
    hardware_gpio
    hardware_adc          # <- add ADC here
    platform
    cyw43_arch
)

# Enable USB/stdio (optional)
pico_enable_stdio_usb(HW2 1)
pico_enable_stdio_uart(HW2 0)

# Generate map/bin/hex/uf2 files
pico_add_extra_outputs(HW2)

# Export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)