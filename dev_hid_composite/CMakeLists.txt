# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Pico SDK setup
set(PICO_BOARD pico2_w CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(dev_hid_composite C CXX ASM)

pico_sdk_init()

add_executable(dev_hid_composite)

target_sources(dev_hid_composite PUBLIC
    main.c
    usb_descriptors.c
    imu.c
)

target_include_directories(dev_hid_composite PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(dev_hid_composite PUBLIC
    pico_stdlib
    pico_unique_id
    tinyusb_device
    tinyusb_board
    hardware_i2c
    m
)

pico_add_extra_outputs(dev_hid_composite)