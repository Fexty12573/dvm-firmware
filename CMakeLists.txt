cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(dvm_firmware C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# Source Files
add_executable(dvm_firmware
    src/main.cpp
    src/DigitalVoltmeter.cpp
    src/AD7177.cpp
    src/Communication.cpp
    src/UART.cpp
)

pico_enable_stdio_usb(dvm_firmware 1)
pico_enable_stdio_uart(dvm_firmware 1)
pico_add_extra_outputs(dvm_firmware)

target_include_directories(dvm_firmware PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(dvm_firmware
    pico_stdlib
    pico_multicore
    hardware_sync
)

add_compile_options(
    -Wall
    -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
    -Wno-unused-function # we have some for the docs that aren't called
    -Wno-maybe-uninitialized
)
