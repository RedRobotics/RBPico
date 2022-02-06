# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "/home/pi/pico/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(rbpico C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(rbpico rbpico.c )

pico_generate_pio_header(rbpico ${CMAKE_CURRENT_LIST_DIR}/pwm.pio)

pico_set_program_name(rbpico "rbpico")
pico_set_program_version(rbpico "0.1")

pico_enable_stdio_uart(rbpico 0)
pico_enable_stdio_usb(rbpico 1)

# Add the standard library to the build
target_link_libraries(rbpico pico_stdlib)

# Add any user requested libraries
target_link_libraries(rbpico
        hardware_i2c
        hardware_pio
        hardware_pwm
        )

pico_add_extra_outputs(rbpico)

