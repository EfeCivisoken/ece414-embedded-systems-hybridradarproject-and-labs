#ifndef TOUCHDISPLAY_H
#define TOUCHDISPLAY_H
#include "pico/stdlib.h"
#include "stdbool.h"
#include "inttypes.h"
// *** HINT: *** This enum defines a unique code for each button on the
calculator
typedef enum Buttons {
    B0, B1, B2, B3, B4, B5, B6, B7, B8, B9, // Digit buttons 0-9
    BPLUS, BMINUS, BMULT, BDIV, // Arithmetic operators: +, -, *, /
    BEQ, BCL, // Equals and Clear
    BERR // Error code for invalid touch
} Buttons;
// Public functions provided by the TouchDisplay module
void drawCalcScreen(); // Draws calculator UI
Buttons getButton(uint16_t x, uint16_t y); // Converts touch to button code
void displayResult(int32_t num); // Displays a number
void displayError(); // Displays "ERROR"
void displayDiv0(); // Displays "DIV0"
#endif