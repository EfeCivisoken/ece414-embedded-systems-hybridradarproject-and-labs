#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "pico/stdlib.h"
#include <stdbool.h>

void Ultrasonic_Init(uint32_t trig_l, uint32_t echo_l, uint32_t trig_r, uint32_t echo_r);
void Ultrasonic_Trigger(bool left);
bool Ultrasonic_IsBusy(void);
float Ultrasonic_GetDistance(void);

#endif
