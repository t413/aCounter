cmake_minimum_required(VERSION 2.8)

set(AVR_UPLOADTOOL avrdude)
set(AVR_PROGRAMMER usbasp)
set(AVR_UPLOADTOOL_PORT usb)

set(AVR_MCU atmega328p)
set(MCU_SPEED "12000000L")
set(AVR_H_FUSE 0xD8)
set(AVR_L_FUSE 0xFF)
set(MCU_MAX_SIZE 28672) #TODO use this!

message(STATUS "Current uploadtool is: ${AVR_UPLOADTOOL}")
message(STATUS "Current programmer is: ${AVR_PROGRAMMER}")
message(STATUS "Current upload port is: ${AVR_UPLOADTOOL_PORT}")
message(STATUS "Current uploadtool options are: ${AVR_UPLOADTOOL_OPTIONS}")
message(STATUS "Current MCU is set to: ${AVR_MCU}")
message(STATUS "Current H_FUSE is set to: ${AVR_H_FUSE}")
message(STATUS "Current L_FUSE is set to: ${AVR_L_FUSE}")

include("generic-gcc-avr.cmake")

project(aCounter)

set(CMAKE_C_FLAGS "-Os -std=gnu99")
set(CMAKE_CXX_FLAGS "-Os")
add_definitions("-DF_CPU=${MCU_SPEED}")
#add_definitions("-fpack-struct")
add_definitions("-fshort-enums")
add_definitions("-Wall")
add_definitions("-Werror")
add_definitions("-pedantic")
add_definitions("-pedantic-errors")
add_definitions("-funsigned-char")
add_definitions("-funsigned-bitfields")
add_definitions("-ffunction-sections")

add_subdirectory(display)
add_subdirectory(lib)
add_subdirectory(main)


