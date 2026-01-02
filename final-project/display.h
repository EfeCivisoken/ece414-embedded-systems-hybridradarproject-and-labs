// display.h
#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>

// Initialize radar drawing state (assumes TFT already initialized)
void Display_Init(void);

// Draw static radar background (arcs, grid, labels)
void Display_DrawBackground(void);

// Update radar with a new reading
//  - left      : true = left sensor, false = right sensor
//  - angle_deg : 0–180 sweep angle
//  - distance_cm : distance in cm (<= 0 means invalid)
//  - motion    : PIR motion detected on that side
void Display_Update(bool left, float angle_deg, float distance_cm, bool motion);

#endif
