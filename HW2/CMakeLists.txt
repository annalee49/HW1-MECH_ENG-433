cmake_minimum_required(VERSION 3.13)


# Set the project name and language
project(HW2 C CXX ASM)

# Path to your Pico SDK (adjust if needed)
set(PICO_SDK_PATH "/path/to/pico-sdk")

# Initialize the SDK
include(pico_sdk_import.cmake)

# Initialize the Pico SDK
pico_sdk_init()

# Add your executable
add_executable(HW2 HW2.c)

# Link the Pico standard library
target_link_libraries(HW2 pico_stdlib)

# Enable USB output (optional, remove if unused)
pico_enable_stdio_usb(HW2 1)
pico_enable_stdio_uart(HW2 0)

# Create map/bin/hex files (optional)
pico_add_extra_outputs(HW2)
