#include "pico/stdlib.h"
#include "TouchDisplay.h"
#include "inputnum.h"
#include "ts_lcd.h"
#include "uart.h"
#include <stdio.h>
// === TODO: === Define states here
enum CALC_STATES {
    CALC_INIT, // Start state
    CALC_FIRST_NUM,// Entering first number
    CALC_OPERATOR,// Operator selected
    CALC_SECOND_NUM,// Entering second number
    CALC_SHOW_RESULT// Show result
} CALC_State;
int32_t num1 = 0, num2 = 0, result = 0;
OPERATOR currentOp = ERR;
int main() {
    stdio_init_all();
    ts_lcd_init();

    CALC_State = CALC_INIT;
    drawCalcScreen();
    displayResult(0);
    while (1) {
        uint16_t touch_x, touch_y;
        bool touched = get_ts_lcd(&touch_x, &touch_y);
        Buttons pressedButton = BERR;
        if (touched) {
            pressedButton = getButton(touch_x, touch_y);
            printf("Button: %d\n", pressedButton); // Debugging
        }
        switch (CALC_State) {
        case CALC_INIT:
        if (pressNumber(pressedButton)) {
        clear();
        num1 = getInput(pressedButton);
        displayResult(num1);
        CALC_State = CALC_FIRST_NUM;
        }
        break;

        case CALC_FIRST_NUM:
        if (pressNumber(pressedButton)) {
            num1 = getInput(pressedButton);
            displayResult(num1);
        } else if (pressOperator(pressedButton)) {
            currentOp = readOperator(pressedButton);
            clear();
            CALC_State = CALC_OPERATOR;
        } else if (pressC(pressedButton)) {
            clear();
            displayResult(0);
            CALC_State = CALC_INIT;
        }
        break;

        case CALC_OPERATOR:
            if (pressNumber(pressedButton)) {
                num2 = getInput(pressedButton);
                displayResult(num2);
                CALC_State = CALC_SECOND_NUM;
            } else if (pressC(pressedButton)) {
                clear();
                displayResult(0);
                CALC_State = CALC_INIT;
            }
            break;

        case CALC_SECOND_NUM:
            if (pressNumber(pressedButton)) {
                num2 = getInput(pressedButton);
                displayResult(num2);
            } else if (pressEq(pressedButton)) {
                result = performOperation(num1, num2, currentOp);
                displayResult(result);
                CALC_State = CALC_SHOW_RESULT;
            } else if (pressC(pressedButton)) {
                clear();
                displayResult(0);
                CALC_State = CALC_INIT;
            }
            break;

            case CALC_SHOW_RESULT:
            if (pressC(pressedButton)) {
                clear();
                displayResult(0);
                CALC_State = CALC_INIT;
            }
            break;
        }

        sleep_ms(200); // Slower for debugging
    }
}