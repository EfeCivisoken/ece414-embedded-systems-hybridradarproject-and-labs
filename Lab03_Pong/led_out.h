// Declares LED control functions, enabling other modules to manipulate ball position and display victory signals.
#ifndef LED_OUT_H
#define LED_OUT_H

#include "pico/stdlib.h"

void led_init();
void led_set(uint8_t pos);    // turn on LED at position, others off
void led_flash(uint8_t pos);  // flash LED at winner’s end

#endif
