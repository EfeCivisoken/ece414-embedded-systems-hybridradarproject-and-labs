#ifndef SERVO_H
#define SERVO_H

#include <stdbool.h>
#include "pico/stdlib.h"  

void Servo_Init(uint32_t gpio_left, uint32_t gpio_right);
void Servo_UpdateSweep(void);
float Servo_GetAngle(bool left);

#endif
