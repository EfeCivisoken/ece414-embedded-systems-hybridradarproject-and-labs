// Declares switch input functions, allowing the FSM to check if players pressed their paddles at the right moment.
#ifndef SW_IN_H
#define SW_IN_H

#include "pico/stdlib.h"
#include <stdbool.h>

void sw_init(uint pin);
bool sw_pressed(uint pin);   // with debouncing

#endif
