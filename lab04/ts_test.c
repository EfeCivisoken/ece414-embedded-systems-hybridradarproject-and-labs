// File: ts_test.c - Code Skeleton
// Prepared by Prof. Priyank Kalgaonkar

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "stdbool.h"
#include "inttypes.h"
#include "TouchScreen.h"
#include "TFTMaster.h"
#include "ts_lcd.h"

// TODO: (Done): Define buffers for storing coordinate strings
static char buffer1[30], buffer2[30];

// TODO: (Done): Define state tracking variables
static bool screenTouched, firstscreenTouched;
static uint16_t last_lcdx, last_lcdy; // Store last touch coordinates

// ---- UI layout & styles (tweak if needed) ----
#define RAW_TXT_X   4
#define RAW_TXT_Y   6
#define LCD_TXT_X   4
#define LCD_TXT_Y   22

// Crosshair per lab = ~11x11 centered (radius 5 → lines from -5..+5)  :contentReference[oaicite:2]{index=2}
#define CH_RADIUS   5
#define CH_COLOR    ILI9340_GREEN
#define BG_COLOR    ILI9340_BLACK
#define LCD_TEXT_COLOR ILI9340_YELLOW
#define RAW_TEXT_COLOR ILI9340_CYAN

void writeRAWCoor(struct TSPoint p) {
    // TODO: (Done): Set cursor position for raw coordinates display
    tft_setCursor(RAW_TXT_X, RAW_TXT_Y);
    // TODO: (Done): Set text color for raw coordinates
    tft_setTextColor(RAW_TEXT_COLOR);
    // TODO: (Done): Format raw coordinates into buffer1
    sprintf(buffer1, "RAW  x:%4d y:%4d z:%4d", p.x, p.y, p.z);
    // TODO: (Done): Display the formatted string
    tft_writeString(buffer1);
}

void writeLCDCoor(uint16_t xcoor, uint16_t ycoor) {
    // TODO: (Done): Set cursor position for LCD coordinates display
    tft_setCursor(LCD_TXT_X, LCD_TXT_Y);
    // TODO: (Done): Set text color for LCD coordinates  
    tft_setTextColor(LCD_TEXT_COLOR);
    // TODO: (Done): Format LCD coordinates into buffer2
    sprintf(buffer2, "LCD  x:%3u  y:%3u", xcoor, ycoor);
    // TODO: (Done): Display the formatted string
    tft_writeString(buffer2);
}

void drawCrossHair(uint16_t xcoor, uint16_t ycoor) {
    // TODO: (Done): Draw a circle at the touch point (10–11 pixel diameter)
    tft_drawCircle(xcoor, ycoor, CH_RADIUS, CH_COLOR);
    // TODO: (Done): Draw horizontal line through center (≈11 pixels long)
    tft_drawLine(xcoor - CH_RADIUS, ycoor, xcoor + CH_RADIUS, ycoor, CH_COLOR);
    // TODO: (Done): Draw vertical line through center (≈11 pixels long)  
    tft_drawLine(xcoor, ycoor - CH_RADIUS, xcoor, ycoor + CH_RADIUS, CH_COLOR);
    // TODO: (Done): Draw a center pixel for better visibility
    tft_drawPixel(xcoor, ycoor, CH_COLOR);
}

void deleteCrossHair(uint16_t xcoor, uint16_t ycoor) {
    // TODO: (Done): Erase the crosshair by drawing over it with background color
    // This creates the "erase" effect by overwriting with background color
    tft_drawCircle(xcoor, ycoor, CH_RADIUS, BG_COLOR);
    tft_drawLine(xcoor - CH_RADIUS, ycoor, xcoor + CH_RADIUS, ycoor, BG_COLOR);
    tft_drawLine(xcoor, ycoor - CH_RADIUS, xcoor, ycoor + CH_RADIUS, BG_COLOR);
    tft_drawPixel(xcoor, ycoor, BG_COLOR);
}

void clearScreen(char bufferx[30], char buffery[30]) {
    // TODO: (Done): Clear the raw coordinates text by overwriting with background color
    tft_setCursor(RAW_TXT_X, RAW_TXT_Y);
    tft_setTextColor(BG_COLOR);
    tft_writeString(bufferx);
    // TODO: (Done): Clear the LCD coordinates text similarly
    tft_setCursor(LCD_TXT_X, LCD_TXT_Y);
    tft_setTextColor(BG_COLOR);
    tft_writeString(buffery);
    // TODO: (Done): Erase the previous crosshair
    if (firstscreenTouched) {
        deleteCrossHair(last_lcdx, last_lcdy);
    }
}

int main() {
    // TODO: (Done): Initialize the touchscreen and LCD system
    stdio_init_all();
    ts_lcd_init();
    tft_setTextSize(1);
    tft_setTextWrap(0);
    
    // TODO: (Done): Initialize the first touch flag to false
    firstscreenTouched = false;
    buffer1[0] = buffer2[0] = '\0';
    
    while (1) {
        // TODO: (Done): Clear previous frame's display elements
        clearScreen(buffer1, buffer2);
        
        // TODO: (Done): Get raw touchscreen coordinates
        struct TSPoint p = {0};
        getPoint(&p);
        
        // TODO: (Done): Display raw coordinates on screen
        writeRAWCoor(p);
        
        // TODO: (Done): Convert to LCD coordinates and check if screen is touched
        uint16_t lcdx = 0, lcdy = 0;
        // NOTE: Lab hint about parameter order (&lcdy, &lcdx) due to wiring/rotation.  :contentReference[oaicite:3]{index=3}
        screenTouched = get_ts_lcd(&lcdy, &lcdx);
        
        // TODO: (Done): Update last coordinates if screen is currently touched
        if (screenTouched) {
            firstscreenTouched = true;
            last_lcdx = lcdx;
            last_lcdy = lcdy;
        }
        
        // TODO: (Done): Display last coordinates and crosshair if screen has ever been touched
        if (firstscreenTouched) {
            writeLCDCoor(last_lcdx, last_lcdy);
            // NOTE: With rotation set as in ts_lcd_init(), these coordinates already match LCD axes.  :contentReference[oaicite:4]{index=4}
            drawCrossHair(last_lcdx, last_lcdy);
        }
        
        // TODO: (Done): Add small delay to prevent flickering
        sleep_ms(100);
    }
}
