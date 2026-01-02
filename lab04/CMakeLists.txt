cmake_minimum_required(VERSION 3.13)

# Pull in the Pico SDK (this file must be in project root)
include(pico_sdk_import.cmake)

project(lab04 C CXX ASM)

# Initialize the SDK
pico_sdk_init()

# ---- Our executable ----
add_executable(ts_test
    ts_lcd.c ## Intermediary kind of driver layer combining hardware drivers of TFTMaster and TouchScreen
    ts_test.c ## this is main in this version
    TFTMaster.c
    TouchScreen.c
    
)

# Generate SPIPIO.pio.h from SPIPIO.pio (target must already exist)
pico_generate_pio_header(ts_test ${CMAKE_CURRENT_LIST_DIR}/SPIPIO.pio)

# Link libs
target_link_libraries(ts_test PRIVATE
    pico_stdlib
    hardware_adc
    hardware_pio        # we use PIO for TFT
    hardware_irq
    
)

# USB stdio on, UART off (matches our printf usage over USB)
pico_enable_stdio_usb(ts_test 1)
pico_enable_stdio_uart(ts_test 0)

# Produce .uf2/.bin/.elf
pico_add_extra_outputs(ts_test)
