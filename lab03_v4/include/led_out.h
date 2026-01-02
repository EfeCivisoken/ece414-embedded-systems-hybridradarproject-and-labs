#pragma once
#include <stdint.h>

// Initializes GPIO2..GPIO9 as outputs and clears them (LEDs off)
void led_out_init(void);

// Writes the 8 bits of 'value' to LEDs on GPIO2..GPIO9
// bit0 -> GPIO2 ... bit7 -> GPIO9
void led_out_write(uint8_t value);
