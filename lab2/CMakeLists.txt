cmake_minimum_required(VERSION 3.13)

# Pull in SDK (expects pico_sdk_import.cmake in repo root or parent)
include(pico_sdk_import.cmake)

project(Lab2 C CXX ASM)
pico_sdk_init()

add_executable(lab2
    main.c
    led_out.c
    sw_in.c
)

# Optional: USB stdio on, UART off (fine to omit)
pico_enable_stdio_usb(lab2 1)
pico_enable_stdio_uart(lab2 0)

# Headers live in the same dir
target_include_directories(lab2 PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# Stretch: double-reset to enter BOOTSEL without replugging
target_link_libraries(lab2
    pico_stdlib
    pico_bootsel_via_double_reset
)

pico_add_extra_outputs(lab2)
