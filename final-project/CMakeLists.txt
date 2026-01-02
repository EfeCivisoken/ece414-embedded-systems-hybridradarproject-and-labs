cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(hybrid_radar_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(hybrid_radar
    main.c
    servo.c
    ultrasonic.c
    motion.c
    buzzer.c
    serial.c
    display.c
    ts_lcd.c
    TFTMaster.c
    TouchScreen.c
)

# Generate header for the PIO-based SPI engine used by TFTMaster
pico_generate_pio_header(hybrid_radar ${CMAKE_CURRENT_LIST_DIR}/SPIPIO.pio)

# Include local headers (servo.h, ultrasonic.h, display.h, etc.)
target_include_directories(hybrid_radar PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link against required libraries
target_link_libraries(hybrid_radar 
    pico_stdlib        # core SDK: time, GPIO, prints, delays
    hardware_pwm       # servos and buzzers
    hardware_gpio      # ultrasonic, PIR, buttons, etc.
    hardware_irq       # ultrasonic echo interrupts
    hardware_adc       # touchscreen analog reads
    hardware_pio       # REQUIRED for TFT SPI (SPIPIO.pio)
    m                  # math library: sinf(), cosf(), sqrtf(), M_PI
)

# Create UF2 + ELF
pico_add_extra_outputs(hybrid_radar)

# Enable USB CDC for serial communication
pico_enable_stdio_usb(hybrid_radar 1)
pico_enable_stdio_uart(hybrid_radar 0)
