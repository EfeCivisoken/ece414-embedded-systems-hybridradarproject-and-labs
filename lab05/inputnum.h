#ifndef INPUTNUM_H 
#define INPUTNUM_H 
 
#include "pico/stdlib.h" 
#include "stdbool.h" 
#include "inttypes.h" 
#include "TouchDisplay.h" 
 
typedef enum OPERATOR {PLUS, MINUS, MULT, DIV, EQ, ERR} OPERATOR; 
 
// Public functions for number input and arithmetic 
void clear(); 
int32_t getInput(Buttons button); 
OPERATOR readOperator(Buttons button); 
int32_t performOperation(int32_t num1, int32_t num2, OPERATOR op); 
bool overLimitResult(int32_t num); 
bool pressC(Buttons button); 
bool pressEq(Buttons button); 
bool pressOperator(Buttons button); 
bool pressNumber(Buttons button); 
 
#endif 
