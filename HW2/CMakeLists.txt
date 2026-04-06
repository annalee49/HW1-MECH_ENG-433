cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(HW2 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Pico SDK
pico_sdk_init()

# Add executable
add_executable(HW2
    main.c
)

# Link Pico SDK libraries
target_link_libraries(HW2
    pico_stdlib       # Core SDK (includes pico/platform.h)
    hardware_pwm      # PWM driver
    hardware_adc      # ADC driver
)

# Enable USB and UART (optional)
pico_enable_stdio_usb(HW2 1)
pico_enable_stdio_uart(HW2 1)

# Generate binary outputs
pico_add_extra_outputs(HW2)