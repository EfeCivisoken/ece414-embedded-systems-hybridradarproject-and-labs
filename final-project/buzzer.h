#ifndef BUZZER_H
#define BUZZER_H

#include "pico/stdlib.h"

void Buzzer_Init(uint32_t pin_left, uint32_t pin_right);
void Buzzer_Update(float dist_left, float dist_right);

#endif
