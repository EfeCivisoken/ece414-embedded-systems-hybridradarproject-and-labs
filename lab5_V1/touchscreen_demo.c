/* 
 * File:   touch_main.c
 * Author: Matthew Watkins
 * Ported to Pico by John Nestor
 * not used just ignore this file
 *
 * Description: Prints out the detected position (x, y) of a press on the
 * touchscreen as well as the pressure (z).
 * 
 * IMPORTANT: For this example to work, you need to make the following 
 * connections from the touchpad to the Pico:
 *  Y+ => ADC1/GP27 (Pin 32)
 *  Y- => GP21 (Pin 27)
 *  X+ => GP22 (Pin 29)
 *  X- => ADC0/GP26 (Pin 31)
 * 
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "TouchScreen.h"
#include "TFTMaster.h"

#define text_box_offset 20
#define vertical_offset 70
#define width 160
#define height 40
#define x_offset 180
#define starting_x 40

enum opperation
{
    times,
    divide,
    add,
    subtract,
    clear,
    solve,
    none
};

String opperations = "";
int first_number = 0;
int second_number = 0;
enum opperation current_op;
bool opperation_selected = false;
bool error_state = false;


int main() {
    char buffer[30];

    adc_init();
    //initialize screen
    tft_init_hw();
    tft_begin();
    tft_setRotation(3);
    tft_fillScreen(ILI9340_BLACK);

    while(1){
        //tft_fillScreen(ILI9341_BLACK);
        tft_setCursor(20, 100);
        tft_setTextColor(ILI9340_WHITE); tft_setTextSize(2);

        //erase old text
        tft_setTextColor(ILI9340_BLACK);
        tft_writeString(buffer);

        struct TSPoint p;
        p.x = 0;
        p.y = 0;
        p.z = 0;
        getPoint(&p);
        //disableing the cursor for now (turn on if needed for debugging)
        //tft_setCursor(20, 100);
        //tft_setTextColor(ILI9340_WHITE);
        //sprintf(buffer,"x: %d, y: %d, z: %d", p.x, p.y, p.z);
        //tft_writeString(buffer);
        drawButtons();
        sleep_ms(100);
    }
}

void prossesInput(int x, int y, int z){
    if(z < 100) return; //ignore if not pressed hard enough
    int output = -1;
    enum opperation op;
    if(x > starting_x && x < starting_x + width){
        if(y > vertical_offset + text_box_offset && y < vertical_offset + text_box_offset + height){
            //button 7
            output = 7;
        } else if(y > vertical_offset * 2 + text_box_offset && y < vertical_offset * 2 + text_box_offset + height){
            //button 4
            output = 4;
        } else if(y > vertical_offset * 3 + text_box_offset && y < vertical_offset * 3 + text_box_offset + height){
            //button 1
            output = 1;
        } else if(y > vertical_offset * 4 + text_box_offset && y < vertical_offset * 4 + text_box_offset + height){
            //button 0
            output = 0;
        }
    } else if(x > starting_x + x_offset && x < starting_x + x_offset + width){
        if(y > vertical_offset + text_box_offset && y < vertical_offset + text_box_offset + height){
            //button 8
            output = 8;
        } else if(y > vertical_offset * 2 + text_box_offset && y < vertical_offset * 2 + text_box_offset + height){
            //button 5
            output = 5;
        } else if(y > vertical_offset * 3 + text_box_offset && y < vertical_offset * 3 + text_box_offset + height){
            //button 2
            output = 2;
        } else if(y > vertical_offset * 4 + text_box_offset && y < vertical_offset * 4 + text_box_offset + height){
            //button C
            op = clear;
            error_state = false;
            opperation_selected = false;
            opperations = "";
            first_number = NULL;
            second_number = NULL;
            current_op = NULL;
        }
    } else if(x > starting_x + x_offset * 2 && x < starting_x + x_offset * 2 + width){
        if(y > vertical_offset + text_box_offset && y < vertical_offset + text_box_offset + height){
            //button 9
            output = 9;
        } else if(y > vertical_offset * 2 + text_box_offset && y < vertical_offset * 2 + text_box_offset + height){
            //button 6
            output = 6;
        } else if(y > vertical_offset * 3 + text_box_offset && y < vertical_offset * 3 + text_box_offset + height){
            //button 3
            output = 3;
        } else if(y > vertical_offset * 4 + text_box_offset && y < vertical_offset * 4 + text_box_offset + height){
            //button =
            op = solve;

        }
    } else if(x > starting_x + x_offset * 3 && x < starting_x + x_offset * 3 + width){
        if(y > vertical_offset + text_box_offset && y < vertical_offset + text_box_offset + height){
            //button +
            op = add;
        } else if(y > vertical_offset * 2 + text_box_offset && y < vertical_offset * 2 + text_box_offset + height){
            //button -
            op = subtract;
        } else if(y > vertical_offset * 3 + text_box_offset && y < vertical_offset * 3 + text_box_offset + height){
            //button *
            op = times;
        } else if(y > vertical_offset * 4 + text_box_offset && y < vertical_offset * 4 + text_box_offset + height){
            //button /
            op = divide;
        }
    }

    if(output != -1){
        //a number was pressed
        opperations += String(output);
        if(opperation_selected){
            second_number = second_number * 10 + output;
        } else {
            first_number = first_number * 10 + output;
        }
    } else {
        //an opperation was pressed
        if(current_op != NULL) {
            opperations = "Error";
            error_state = true;
            return; //ignore if no first number or opperation
        }
        switch(op){
            case add:
                opperations += "+";
                current_op = add;
                opperation_selected = true;
                break;
            case subtract:
                opperations += "-";
                current_op = subtract;
                opperation_selected = true;
                break;
            case times:
                opperations += "*";
                current_op = times;
                opperation_selected = true;
                break;
            case divide:
                opperations += "/";
                current_op = divide;
                opperation_selected = true;
                break;
            case clear:
                opperations = "";
                current_op = NULL;
                first_number = 0;
                second_number = 0;
                error_state = false;
                opperation_selected = false;
                break;
            case solve:
                if(!opperation_selected){ //ignore if no opperation selected
                    opperations = "Error";
                    error_state = true;
                    return;
                }
                switch(current_op){
                    case add:
                        first_number += second_number;
                        break;
                    case subtract:
                        first_number -= second_number;
                        break;
                    case times:
                        first_number *= second_number;
                        break;
                    case divide:
                        if(second_number == 0){
                            opperations = "Error";
                            error_state = true;
                            return;
                        }
                        first_number /= second_number;
                        break;
                    default:
                        opperations = "Error";
                        error_state = true;
                        return;
                }
                second_number = 0;
                current_op = NULL;
                break;
        }
    }
}

void button(int x, int y, int w, int h, char * label){
    tft_fillRoundRect(x,y,w,h,5,ILI9340_RED);
    tft_drawRoundRect(x,y,w,h,5,ILI9340_WHITE);
    tft_setCursor(x+10,y+h/2-8);
    tft_setTextColor(ILI9340_WHITE); tft_setTextSize(2);
    tft_writeString(label);
}

void drawButtons(void){
    tft_fillScreen(ILI9340_BLACK);
    button(starting_x, vertical_offset + text_box_offset, width, height, "7");
    button(starting_x, vertical_offset * 2 + text_box_offset, width, height, "4");
    button(starting_x, vertical_offset * 3 + text_box_offset, width, height, "1");
    button(starting_x, vertical_offset * 4 + text_box_offset, width, height, "0");
    button(starting_x + x_offset, vertical_offset + text_box_offset, width, height, "8");
    button(starting_x + x_offset, vertical_offset * 2 + text_box_offset, width, height, "5");
    button(starting_x + x_offset, vertical_offset * 3 + text_box_offset, width, height, "2");
    button(starting_x + x_offset, vertical_offset * 4 + text_box_offset, width, height, "C");
    button(starting_x + x_offset * 2, vertical_offset + text_box_offset, width, height, "9");
    button(starting_x + x_offset * 2, vertical_offset * 2 + text_box_offset, width, height, "6");
    button(starting_x + x_offset * 2, vertical_offset * 3 + text_box_offset, width, height, "3");
    button(starting_x + x_offset * 2, vertical_offset * 4 + text_box_offset, width, height, "=");
    button(starting_x + x_offset * 3, vertical_offset + text_box_offset, width, height, "+");
    button(starting_x + x_offset * 3, vertical_offset * 2 + text_box_offset, width, height, "-");
    button(starting_x + x_offset * 3, vertical_offset * 3 + text_box_offset, width, height, "*");
    button(starting_x + x_offset * 3, vertical_offset * 4 + text_box_offset, width, height, "/");
}

