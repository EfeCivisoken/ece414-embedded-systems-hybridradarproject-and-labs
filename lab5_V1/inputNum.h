#ifndef INPUTNUM_H
#define INPUTNUM_H
#include "pico/stdlib.h"
#include "stdbool.h"
#include "inttypes.h"
#include "TouchDisplay.h"
typedef enum OPERATOR {PLUS, MINUS, MULT, DIV, EQ, ERR} OPERATOR;
// Public functions for number input and arithmetic
void clear(); // Resets input module
int32_t getInput(Buttons button); // Processes digit press
OPERATOR readOperator(Buttons button); // Converts button to OPERATOR
int32_t performOperation(int32_t num1, int32_t num2, OPERATOR op);
bool overLimitResult(int32_t num); // Checks for overflow
bool pressC(Buttons button); // Checks if Clear pressed
bool pressEq(Buttons button); // Checks if Equals pressed
bool pressOperator(Buttons button); // Checks if operator pressed
bool pressNumber(Buttons button); // Checks if digit pressed
#endif