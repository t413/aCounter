project(main)

include_directories(${lib_SOURCE_DIR})
include_directories(${display_SOURCE_DIR})

add_avr_executable(
   aCounter
   main.cpp
)

# find_library( C_LIB avr-libc)
# message(STATUS "avr-libc: ${C_LIB}")

avr_target_link_libraries(aCounter display acounter_lib) #${C_LIB})
