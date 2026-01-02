# Configures the Pico Pong project build, includes the SDK, adds source files, and generates outputs.

cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)

project(pong_game C CXX ASM)
pico_sdk_init()

add_executable(pong
    main.c
    fsm.c
    fsm.h
    led_out.c
    led_out.h
    sw_in.c
    sw_in.h
)

target_link_libraries(pong pico_stdlib)
pico_add_extra_outputs(pong)
