project(display)

include_directories(${acounter_lib_SOURCE_DIR})

add_avr_library(
   display
   oled.cpp
   oled.h
)

