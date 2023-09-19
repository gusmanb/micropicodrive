# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "F:/PicoSDK/Pico/pico-sdk")

set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(MicroPicoDrive C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

# add_executable(MicroPicoDrive MicroPicoDrive.c )
FILE(GLOB CSources *.c)
ADD_EXECUTABLE(MicroPicoDrive ${CSources} ssd1306/ssd1306.c pff/diskio.c pff/pff.c)

# Create C header file with the name <pio program>.pio.h
pico_generate_pio_header(${PROJECT_NAME}  
        ${CMAKE_CURRENT_LIST_DIR}/PIO_machines.pio
)

pico_set_program_name(MicroPicoDrive "MicroPicoDrive")
pico_set_program_version(MicroPicoDrive "0.1")

pico_enable_stdio_uart(MicroPicoDrive 0)
pico_enable_stdio_usb(MicroPicoDrive 0)

# Add the standard library to the build
target_link_libraries(MicroPicoDrive
        pico_stdlib
        pico_multicore)

# Add the standard include files to the build
target_include_directories(MicroPicoDrive PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# Add any user requested libraries
target_link_libraries(MicroPicoDrive
        hardware_spi
        hardware_pio
        hardware_dma
        hardware_timer
        hardware_i2c)

pico_add_extra_outputs(MicroPicoDrive)

