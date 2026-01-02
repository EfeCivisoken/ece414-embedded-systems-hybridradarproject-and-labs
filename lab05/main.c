#include "pico/stdlib.h"
#include "TouchDisplay.h"
#include "inputnum.h"
#include "ts_lcd.h"
#include <stdio.h>
#include <limits.h>

// --- Safety color definitions (in case not defined in TFTMaster.h) ---
#ifndef ILI9340_BLACK
#define ILI9340_BLACK   0x0000
#endif
#ifndef ILI9340_BLUE
#define ILI9340_BLUE    0x001F
#endif
#ifndef ILI9340_RED
#define ILI9340_RED     0xF800
#endif
#ifndef ILI9340_GREEN
#define ILI9340_GREEN   0x07E0
#endif
#ifndef ILI9340_WHITE
#define ILI9340_WHITE   0xFFFF
#endif
#ifndef ILI9340_YELLOW
#define ILI9340_YELLOW  0xFFE0
#endif
#ifndef ILI9340_ORANGE
#define ILI9340_ORANGE  0xFD20
#endif
#ifndef ILI9340_DARKGREY
#define ILI9340_DARKGREY 0x7BEF
#endif

// === Calculator States ===
enum CALC_STATES {
    CALC_INIT,
    CALC_FIRST_NUM,
    CALC_OPERATOR,
    CALC_SECOND_NUM,
    CALC_SHOW_RESULT,
    CALC_ERROR,
    CALC_DIV0
} CALC_State;

int32_t num1 = 0, num2 = 0, result = 0;
OPERATOR currentOp = ERR;

int main() {
    stdio_init_all();
    ts_lcd_init();

    CALC_State = CALC_INIT;
    drawCalcScreen();
    displayResult(0);

    bool wasDown = false;          // one-press-per-touch
    Buttons pressedButton = BERR;

    while (1) {
        uint16_t x = 0, y = 0;
        bool isDown = get_ts_lcd(&x, &y);
        pressedButton = BERR;

        // Rising-edge: register one button press
        if (isDown && !wasDown) {
            pressedButton = getButton(x, y);
            printf("Touch: X=%d Y=%d  Button=%d\n", x, y, pressedButton);
        }
        wasDown = isDown;

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
                } 
                else if (pressOperator(pressedButton)) {
                    // Allow operator switching before second number is entered
                    currentOp = readOperator(pressedButton);
                    displayResult(num1);
                    tft_drawRect(260, 55, 50, 48, ILI9340_WHITE); 
                    sleep_ms(100);
                    tft_drawRect(260, 55, 50, 48, ILI9340_BLACK);
                }
                else if (pressC(pressedButton)) {
                    clear();
                    displayResult(0);
                    CALC_State = CALC_INIT;
                }
                break;
            
            case CALC_SECOND_NUM:
                if (pressNumber(pressedButton)) {
                    num2 = getInput(pressedButton);
                    displayResult(num2);
                }

                // === NEW: allow chained operations like 3×3×3×3=81 ===
                else if (pressOperator(pressedButton)) {
                    if (currentOp != ERR) {
                        if (currentOp == DIV && num2 == 0) {
                            CALC_State = CALC_DIV0;
                            displayDiv0();
                            break;
                        }
                        int64_t tmp = 0;
                        bool overflow = false;
                        switch (currentOp) {
                            case PLUS:  tmp = (int64_t)num1 + num2; break;
                            case MINUS: tmp = (int64_t)num1 - num2; break;
                            case MULT:  tmp = (int64_t)num1 * num2; break;
                            case DIV:   tmp = num1 / num2; break;
                            default: break;
                        }
                        overflow = (tmp > INT32_MAX || tmp < INT32_MIN);
                        if (overflow) {
                            CALC_State = CALC_ERROR;
                            displayError();
                            break;
                        }

                        result = (int32_t)tmp;
                        num1 = result;
                        displayResult(result);
                        clear(); // reset for next operand
                    }
                    currentOp = readOperator(pressedButton);
                    CALC_State = CALC_OPERATOR;
                }
                // === end of chain logic ===
                
                else if (pressEq(pressedButton)) {
                    // ---- Explicit DIV0 and overflow pre-checks ----
                    if (currentOp == DIV && num2 == 0) {
                        CALC_State = CALC_DIV0;
                        displayDiv0();
                        break;
                    }

                    bool overflow = false;
                    int64_t tmp = 0;

                    switch (currentOp) {
                        case PLUS:
                            tmp = (int64_t)num1 + (int64_t)num2;
                            overflow = (tmp > INT32_MAX) || (tmp < INT32_MIN);
                            break;
                        case MINUS:
                            tmp = (int64_t)num1 - (int64_t)num2;
                            overflow = (tmp > INT32_MAX) || (tmp < INT32_MIN);
                            break;
                        case MULT:
                            tmp = (int64_t)num1 * (int64_t)num2;
                            overflow = (tmp > INT32_MAX) || (tmp < INT32_MIN);
                            break;
                        case DIV:
                            overflow = (num1 == INT32_MIN && num2 == -1);
                            break;
                        default:
                            break;
                    }

                    if (overflow) {
                        CALC_State = CALC_ERROR;
                        displayError();
                        break;
                    }

                    result = performOperation(num1, num2, currentOp);
                    displayResult(result);
                    CALC_State = CALC_SHOW_RESULT;
                } 
                else if (pressC(pressedButton)) {
                    clear();
                    displayResult(0);
                    CALC_State = CALC_INIT;
                }
                break;

            case CALC_SHOW_RESULT:
                if (pressOperator(pressedButton)) {
                    currentOp = readOperator(pressedButton);
                    num1 = result;
                    clear();
                    CALC_State = CALC_OPERATOR;
                } else if (pressNumber(pressedButton)) {
                    clear();
                    num1 = getInput(pressedButton);
                    displayResult(num1);
                    CALC_State = CALC_FIRST_NUM;
                } else if (pressC(pressedButton)) {
                    clear();
                    displayResult(0);
                    CALC_State = CALC_INIT;
                }
                break;

            case CALC_ERROR:
            case CALC_DIV0:
                if (pressC(pressedButton)) {
                    clear();
                    displayResult(0);
                    CALC_State = CALC_INIT;
                }
                break;
        }

        // Wait until release to avoid repeats
        if (pressedButton != BERR) {
            while (get_ts_lcd(&x, &y)) sleep_ms(20);
        }

        sleep_ms(50);
    }
}
