// display.c – 360° radar drawing layer for TFT
#include "display.h"
#include "TFTMaster.h"
#include "pico/time.h"

#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Logical LCD size (matches rotation=3 in ts_lcd_init)
static int LCD_W = 320;
static int LCD_H = 240;

// Radar origin (screen center), radius
static int ORIGIN_X;
static int ORIGIN_Y;
#define RADIUS_MAX  110

// History trail — longer fade
#define TRAIL_LENGTH  35

// Colors
#define COLOR_GRID    0x07E0      // green
#define COLOR_TEXT    0xFFFF      // white
#define COLOR_SWEEP   0xFFFF      // white
#define COLOR_MOTION  0xF800      // red
#define BG_COLOR      ILI9340_BLACK

typedef struct {
    bool  valid;
    bool  left;
    float angle_deg;     // mapped into radar space (0–360)
    float distance_cm;
    bool  motion;
} RadarPoint;

static RadarPoint history[TRAIL_LENGTH];
static int history_head = 0;


// -------------------------------------------------------------
// Fade helper
// -------------------------------------------------------------
static uint16_t fade_color(uint16_t base, float factor) {
    if (factor < 0.0f) factor = 0.0f;
    if (factor > 1.0f) factor = 1.0f;

    uint8_t r = (base >> 11) & 0x1F;
    uint8_t g = (base >> 5)  & 0x3F;
    uint8_t b = (base       ) & 0x1F;

    r = (uint8_t)(r * factor);
    g = (uint8_t)(g * factor);
    b = (uint8_t)(b * factor);

    return (uint16_t)((r << 11) | (g << 5) | b);
}


// -------------------------------------------------------------
// Unified fading for both motion & non-motion
// long, smooth, clear
// -------------------------------------------------------------
static uint16_t age_color(float age, bool motion) {

    // Fade fully ends at age = 0.60
    if (age < 0.25f) return 0xFFE0;  // yellow
    if (age < 0.45f) return 0x07E0;  // green
    if (age < 0.60f) return 0x07FF;  // cyan

    // After 0.60 → no dot
    return 0;
}


// -------------------------------------------------------------
// Draw a larger dot (radius = 3)
// -------------------------------------------------------------
static void draw_polar_point(float angle_deg, float dist_cm, uint16_t color) {
    if (dist_cm <= 0.0f)
        return;

    while (angle_deg < 0.0f)   angle_deg += 360.0f;
    while (angle_deg >= 360.0f) angle_deg -= 360.0f;

    float rad = (angle_deg - 90.0f) * (float)M_PI / 180.0f;

    float max_range_cm = 600.0f;
    if (dist_cm > max_range_cm)
        return;

    float r = (dist_cm / max_range_cm) * RADIUS_MAX;

    int x = (int)(ORIGIN_X + r * cosf(rad));
    int y = (int)(ORIGIN_Y - r * sinf(rad));

    // Bigger dot radius = 3
    for (int dx = -3; dx <= 3; dx++) {
        for (int dy = -3; dy <= 3; dy++) {
            if (dx*dx + dy*dy <= 9) {
                int px = x + dx;
                int py = y + dy;
                if (px >= 0 && px < LCD_W && py >= 0 && py < LCD_H)
                    tft_drawPixel(px, py, color);
            }
        }
    }
}


// -------------------------------------------------------------
// Draw full radar grid
// -------------------------------------------------------------
static void draw_grid(void) {
    tft_fillScreen(BG_COLOR);

    tft_fillCircle(ORIGIN_X, ORIGIN_Y, 3, COLOR_GRID);

    for (int i = 1; i <= 4; ++i) {
        int r = (RADIUS_MAX * i) / 4;
        for (int deg = 0; deg < 360; ++deg) {
            float rad = deg * (float)M_PI / 180.0f;
            int x = (int)(ORIGIN_X + r * cosf(rad));
            int y = (int)(ORIGIN_Y - r * sinf(rad));
            if (x >= 0 && x < LCD_W && y >= 0 && y < LCD_H)
                tft_drawPixel(x, y, COLOR_GRID);
        }
    }

    tft_drawLine(ORIGIN_X, ORIGIN_Y - RADIUS_MAX,
                 ORIGIN_X, ORIGIN_Y + RADIUS_MAX, COLOR_GRID);
    tft_drawLine(ORIGIN_X - RADIUS_MAX, ORIGIN_Y,
                 ORIGIN_X + RADIUS_MAX, ORIGIN_Y, COLOR_GRID);

    tft_setTextSize(1);
    tft_setTextColor(COLOR_TEXT);
    tft_setCursor(5, 5);
    tft_writeString("Hybrid Ultrasonic Radar (360 deg)");
    tft_setCursor(5, 18);
    tft_writeString("Range: 0 - 6 m");
}


// -------------------------------------------------------------
// PUBLIC API
// -------------------------------------------------------------
void Display_Init(void) {
    tft_setRotation(3);

    LCD_W = 320;
    LCD_H = 240;

    ORIGIN_X = LCD_W / 2;
    ORIGIN_Y = LCD_H / 2;

    memset(history, 0, sizeof(history));
    history_head = 0;
}

void Display_DrawBackground(void) {
    draw_grid();
}

void Display_Update(bool left, float angle_deg, float distance_cm, bool motion) {

    float mapped_angle = left ? angle_deg : (180.0f + angle_deg);

    while (mapped_angle < 0.0f)   mapped_angle += 360.0f;
    while (mapped_angle >= 360.0f) mapped_angle -= 360.0f;

    history[history_head].valid        = (distance_cm > 0.0f);
    history[history_head].left         = left;
    history[history_head].angle_deg    = mapped_angle;
    history[history_head].distance_cm  = distance_cm;
    history[history_head].motion       = motion;
    history_head = (history_head + 1) % TRAIL_LENGTH;

    draw_grid();

    // draw historical fading dots
    for (int i = 0; i < TRAIL_LENGTH; ++i) {
        int idx = (history_head + i) % TRAIL_LENGTH;
        if (!history[idx].valid) continue;

        float age = (float)i / TRAIL_LENGTH;
        uint16_t c = age_color(age, false);
        if (c == 0) continue;

        draw_polar_point(history[idx].angle_deg,
                         history[idx].distance_cm,
                         c);

        // draw cross if motion
        if (history[idx].motion) {
            float rad = (history[idx].angle_deg - 90.0f) * (float)M_PI / 180.0f;
            float r = (history[idx].distance_cm / 600.0f) * RADIUS_MAX;

            int x = (int)(ORIGIN_X + r * cosf(rad));
            int y = (int)(ORIGIN_Y - r * sinf(rad));

            // cross shape
            tft_drawPixel(x-2, y-2, COLOR_MOTION);
            tft_drawPixel(x-1, y-1, COLOR_MOTION);
            tft_drawPixel(x,   y,   COLOR_MOTION);
            tft_drawPixel(x+1, y+1, COLOR_MOTION);
            tft_drawPixel(x+2, y+2, COLOR_MOTION);

            tft_drawPixel(x-2, y+2, COLOR_MOTION);
            tft_drawPixel(x-1, y+1, COLOR_MOTION);
            tft_drawPixel(x+1, y-1, COLOR_MOTION);
            tft_drawPixel(x+2, y-2, COLOR_MOTION);
        }
    }

    // Draw latest point (bright)
    draw_polar_point(mapped_angle, distance_cm, COLOR_SWEEP);

    if (motion) {
        float rad = (mapped_angle - 90.0f) * (float)M_PI / 180.0f;
        float r = (distance_cm / 600.0f) * RADIUS_MAX;

        int x = (int)(ORIGIN_X + r * cosf(rad));
        int y = (int)(ORIGIN_Y - r * sinf(rad));

        // Cross for current point
        tft_drawPixel(x-2, y-2, COLOR_MOTION);
        tft_drawPixel(x-1, y-1, COLOR_MOTION);
        tft_drawPixel(x,   y,   COLOR_MOTION);
        tft_drawPixel(x+1, y+1, COLOR_MOTION);
        tft_drawPixel(x+2, y+2, COLOR_MOTION);

        tft_drawPixel(x-2, y+2, COLOR_MOTION);
        tft_drawPixel(x-1, y+1, COLOR_MOTION);
        tft_drawPixel(x+1, y-1, COLOR_MOTION);
        tft_drawPixel(x+2, y-2, COLOR_MOTION);
    }
}
