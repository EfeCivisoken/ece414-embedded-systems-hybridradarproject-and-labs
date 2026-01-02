#include "TouchDisplay.h"
#include "TFTMaster.h"
#include "ts_lcd.h"
#include <stdio.h>
// === TODO: === Define button positions (4x4 grid). Below is an example.

#define text_box_offset 20
#define vertical_offset 70
#define width 160
#define height 40
#define x_offset 180
#define starting_x 40

static char displayBuffer[20];
void drawCalcScreen() {
    tft_fillScreen(ILI9340_BLACK);
    tft_setTextSize(2);

    // Display area
    tft_fillRect(10, 10, 300, 40, ILI9340_DARKGREY);
    drawButtons();
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

Buttons getButton(uint16_t x, uint16_t y) {
    // === TODO: === Simple button detection
    // Check if within button 1 area
    if(x > starting_x && x < starting_x + width){
        if(y > vertical_offset + text_box_offset && y < vertical_offset + text_box_offset + height){
            //button 7
            return B7;
        } else if(y > vertical_offset * 2 + text_box_offset && y < vertical_offset * 2 + text_box_offset + height){
            //button 4
            return B4;
        } else if(y > vertical_offset * 3 + text_box_offset && y < vertical_offset * 3 + text_box_offset + height){
            //button 1
            return B1;
        } else if(y > vertical_offset * 4 + text_box_offset && y < vertical_offset * 4 + text_box_offset + height){
            //button 0
            return B0;
        }
    } else if(x > starting_x + x_offset && x < starting_x + x_offset + width){
        if(y > vertical_offset + text_box_offset && y < vertical_offset + text_box_offset + height){
            //button 8
            return B8;
        } else if(y > vertical_offset * 2 + text_box_offset && y < vertical_offset * 2 + text_box_offset + height){
            //button 5
            return B5;
        } else if(y > vertical_offset * 3 + text_box_offset && y < vertical_offset * 3 + text_box_offset + height){
            //button 2
            return B2;
        } else if(y > vertical_offset * 4 + text_box_offset && y < vertical_offset * 4 + text_box_offset + height){
            //button C
            return BCL;
        }
    } else if(x > starting_x + x_offset * 2 && x < starting_x + x_offset * 2 + width){
        if(y > vertical_offset + text_box_offset && y < vertical_offset + text_box_offset + height){
            //button 9
            return B9;
        } else if(y > vertical_offset * 2 + text_box_offset && y < vertical_offset * 2 + text_box_offset + height){
            //button 6
            return B6;
        } else if(y > vertical_offset * 3 + text_box_offset && y < vertical_offset * 3 + text_box_offset + height){
            //button 3
            return B3;
        } else if(y > vertical_offset * 4 + text_box_offset && y < vertical_offset * 4 + text_box_offset + height){
            //button =
            return BEQ;
        }
    } else if(x > starting_x + x_offset * 3 && x < starting_x + x_offset * 3 + width){
        if(y > vertical_offset + text_box_offset && y < vertical_offset + text_box_offset + height){
            //button +
            return BPLUS;
        } else if(y > vertical_offset * 2 + text_box_offset && y < vertical_offset * 2 + text_box_offset + height){
            //button -
            return BMINUS;
        } else if(y > vertical_offset * 3 + text_box_offset && y < vertical_offset * 3 + text_box_offset + height){
            //button *
            return BMULT;
        } else if(y > vertical_offset * 4 + text_box_offset && y < vertical_offset * 4 + text_box_offset + height){
            //button /
            return BDIV;
        }
    }

    return BERR;
}

void displayResult(int32_t num) {
 tft_fillRect(10, 10, 300, 40, ILI9340_DARKGREY);
 tft_setCursor(15, 20);
 tft_setTextColor(ILI9340_WHITE, ILI9340_DARKGREY);
 sprintf(displayBuffer, "%ld", num);
 tft_writeString(displayBuffer);
}
void displayError() {
 tft_fillRect(10, 10, 300, 40, ILI9340_DARKGREY);
 tft_setCursor(15, 20);
 tft_setTextColor(ILI9340_RED, ILI9340_DARKGREY);
 tft_writeString("ERROR");
}