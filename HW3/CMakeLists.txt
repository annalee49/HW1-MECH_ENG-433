# Generated CMake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the path to the official Pico SDK
if(NOT DEFINED PICO_SDK_PATH)
    set(PICO_SDK_PATH "/Users/annalee/.pico-sdk/sdk/2.2.0")
endif()

# Pull in the official SDK
include(${PICO_SDK_PATH}/cmake/pico_sdk_import.cmake)


# ========================================
# Board settings
set(PICO_BOARD pico2_w CACHE STRING "Board type")

# ========================================
# Pull in Raspberry Pi Pico SDK (must be before project)

include(pico_sdk_import.cmake)
# ========================================
project(HW3 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# ========================================
# Add executable
add_executable(HW3 HW3.c)

pico_set_program_name(HW3 "HW3")
pico_set_program_version(HW3 "0.1")

# Enable USB and UART output
pico_enable_stdio_uart(HW3 0)
pico_enable_stdio_usb(HW3 1)

# Link standard library and Pico W Wi-Fi architecture
target_link_libraries(HW3 pico_stdlib pico_cyw43_arch_poll)

# Include project headers
target_include_directories(HW3 PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# Add extra outputs (.uf2, .bin, etc.)
pico_add_extra_outputs(HW3)