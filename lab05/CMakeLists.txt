cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(lab5_calculator) # Change to your project name

pico_sdk_init()

add_executable(lab5_calculator) # Change to your project name

pico_generate_pio_header(lab5_calculator ${CMAKE_CURRENT_LIST_DIR}/SPIPIO.pio)
# Ensure SPIPIO.pio is in your directory

target_sources(lab5_calculator PRIVATE  
    main.c 
    TouchDisplay.c 
    inputnum.c 
    ts_lcd.c 
    TouchScreen.c 
    TFTMaster.c 
    glcdfont.c 
     
)

target_link_libraries(lab5_calculator PRIVATE  
    pico_stdlib  
    hardware_adc  
    hardware_gpio  
    hardware_pio  
    hardware_irq     
    pico_bootsel_via_double_reset 
)

pico_add_extra_outputs(lab5_calculator)
