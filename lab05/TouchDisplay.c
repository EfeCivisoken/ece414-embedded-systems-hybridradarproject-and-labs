#include "TouchDisplay.h"
#include "TFTMaster.h"
#include "ts_lcd.h"
#include <stdio.h>
#include <string.h>

#ifndef ILI9340_DARKGREY
#define ILI9340_DARKGREY 0x7BEF
#endif
#ifndef ILI9340_ORANGE
#define ILI9340_ORANGE   0xFD20
#endif

// --- logical size for rotation 3 (landscape) ---
#define LCD_W 320
#define LCD_H 240

// --- display bar ---
#define DISP_X  6
#define DISP_Y  6
#define DISP_W  (LCD_W - 12)
#define DISP_H  34

// --- adjusted button geometry ---
#define BTN_W   62      // a bit wider (fills horizontally)
#define BTN_H   42      // slightly shorter (fits vertically)
#define GAP_X   7
#define GAP_Y   7
#define COL1_X  10
#define COL2_X  (COL1_X + BTN_W + GAP_X)
#define COL3_X  (COL2_X + BTN_W + GAP_X)
#define COL4_X  (COL3_X + BTN_W + GAP_X)   // ends near x≈283..314

#define ROW1_Y  55
#define ROW2_Y  (ROW1_Y + BTN_H + GAP_Y)
#define ROW3_Y  (ROW2_Y + BTN_H + GAP_Y)
#define ROW4_Y  (ROW3_Y + BTN_H + GAP_Y)   // ends near y≈228 (visible)

#define TEXT_SIZE 2
static inline int char_w(void){ return 6*TEXT_SIZE; }
static inline int char_h(void){ return 8*TEXT_SIZE; }

static char displayBuffer[24];

// --- top display text ---
static void displayStringRightAligned(const char* s, uint16_t color) {
    tft_fillRect(DISP_X, DISP_Y, DISP_W, DISP_H, ILI9340_DARKGREY);
    int len = (int)strlen(s);
    int px_w = len * char_w();
    int x = DISP_X + DISP_W - 4 - px_w;
    if (x < DISP_X + 2) x = DISP_X + 2;
    int y = DISP_Y + (DISP_H - char_h())/2;
    tft_setCursor(x, y);
    tft_setTextSize(TEXT_SIZE);
    tft_setTextColor(color);
    tft_writeString((char*)s);
}

void displayResult(int32_t num){
    snprintf(displayBuffer, sizeof(displayBuffer), "%ld", (long)num);
    displayStringRightAligned(displayBuffer, ILI9340_WHITE);
}
void displayError(){ displayStringRightAligned("ERROR", ILI9340_RED); }
void displayDiv0(){  displayStringRightAligned("DIV0",  ILI9340_YELLOW); }

// --- button draw helper ---
static void drawButton(int x,int y,const char* label,uint16_t fill,uint16_t fg){
    tft_fillRect(x, y, BTN_W, BTN_H, fill);
    int lx = x + (BTN_W - (int)strlen(label)*char_w())/2;
    int ly = y + (BTN_H - char_h())/2;
    tft_setCursor(lx, ly);
    tft_setTextSize(TEXT_SIZE);
    tft_setTextColor(fg);
    tft_writeString((char*)label);
}

// --- draw whole keypad ---
void drawCalcScreen(){
    tft_fillScreen(ILI9340_BLACK);
    tft_fillRect(DISP_X, DISP_Y, DISP_W, DISP_H, ILI9340_DARKGREY);
    tft_drawRect(DISP_X, DISP_Y, DISP_W, DISP_H, ILI9340_WHITE);

    drawButton(COL1_X, ROW1_Y, "1", ILI9340_BLUE, ILI9340_WHITE);
    drawButton(COL2_X, ROW1_Y, "2", ILI9340_BLUE, ILI9340_WHITE);
    drawButton(COL3_X, ROW1_Y, "3", ILI9340_BLUE, ILI9340_WHITE);

    drawButton(COL1_X, ROW2_Y, "4", ILI9340_BLUE, ILI9340_WHITE);
    drawButton(COL2_X, ROW2_Y, "5", ILI9340_BLUE, ILI9340_WHITE);
    drawButton(COL3_X, ROW2_Y, "6", ILI9340_BLUE, ILI9340_WHITE);

    drawButton(COL1_X, ROW3_Y, "7", ILI9340_BLUE, ILI9340_WHITE);
    drawButton(COL2_X, ROW3_Y, "8", ILI9340_BLUE, ILI9340_WHITE);
    drawButton(COL3_X, ROW3_Y, "9", ILI9340_BLUE, ILI9340_WHITE);

    drawButton(COL1_X, ROW4_Y, "0", ILI9340_BLUE,  ILI9340_WHITE);
    drawButton(COL2_X, ROW4_Y, "C", ILI9340_RED,   ILI9340_WHITE);
    drawButton(COL3_X, ROW4_Y, "=", ILI9340_GREEN, ILI9340_WHITE);

    drawButton(COL4_X, ROW1_Y, "+", ILI9340_ORANGE, ILI9340_WHITE);
    drawButton(COL4_X, ROW2_Y, "-", ILI9340_ORANGE, ILI9340_WHITE);
    drawButton(COL4_X, ROW3_Y, "*", ILI9340_ORANGE, ILI9340_WHITE);
    drawButton(COL4_X, ROW4_Y, "/", ILI9340_ORANGE, ILI9340_WHITE);
}

// --- button hit test ---
static inline bool in_rect(uint16_t x,uint16_t y,int rx,int ry){
    return (x>=rx)&&(x<rx+BTN_W)&&(y>=ry)&&(y<ry+BTN_H);
}

Buttons getButton(uint16_t x,uint16_t y){
    if(in_rect(x,y,COL1_X,ROW1_Y))return B1;
    if(in_rect(x,y,COL2_X,ROW1_Y))return B2;
    if(in_rect(x,y,COL3_X,ROW1_Y))return B3;

    if(in_rect(x,y,COL1_X,ROW2_Y))return B4;
    if(in_rect(x,y,COL2_X,ROW2_Y))return B5;
    if(in_rect(x,y,COL3_X,ROW2_Y))return B6;

    if(in_rect(x,y,COL1_X,ROW3_Y))return B7;
    if(in_rect(x,y,COL2_X,ROW3_Y))return B8;
    if(in_rect(x,y,COL3_X,ROW3_Y))return B9;

    if(in_rect(x,y,COL1_X,ROW4_Y))return B0;
    if(in_rect(x,y,COL2_X,ROW4_Y))return BCL;
    if(in_rect(x,y,COL3_X,ROW4_Y))return BEQ;

    if(in_rect(x,y,COL4_X,ROW1_Y))return BPLUS;
    if(in_rect(x,y,COL4_X,ROW2_Y))return BMINUS;
    if(in_rect(x,y,COL4_X,ROW3_Y))return BMULT;
    if(in_rect(x,y,COL4_X,ROW4_Y))return BDIV;

    return BERR;
}
