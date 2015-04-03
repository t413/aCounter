project(acounter_lib)

add_avr_library(
   acounter_lib
   analog.c
   analog.h
   i2c_avr.cpp
   i2c_avr.h
   interrupt_timer.c
   interrupt_timer.h
)

