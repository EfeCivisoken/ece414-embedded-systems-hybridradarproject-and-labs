#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "inttypes.h"
#include "stdbool.h"
#include "TouchScreen.h"
#include "TFTMaster.h"
#include "ts_lcd.h"

// ===================== USER-TUNABLE CALIBRATION =====================
// Raw ranges from your board (measure corners in the raw demo)
#define TS_MIN_X  550
#define TS_MAX_X  3650
#define TS_MIN_Y  550
#define TS_MAX_Y  3650

// On this lab's hardware, lower Z means more pressure (keep this =1).
#define Z_LOWER_IS_MORE_PRESSURE 1
#define PRESSURE_THRESHOLD 850

#define LCD_ROTATION 3

// When setRotation(3), logical screen is 320x240 (x:0..319, y:0..239)
#define ADAFRUIT_LCD_MAX_X 319
#define ADAFRUIT_LCD_MAX_Y 239

// Wiring on many ILI9340 shields yields swapped axes and an inverted Y in rot=3.
// Start with these; flip if your touches look mirrored.
#define TS_SWAP_XY 1
#define TS_INVERT_X 0
#define TS_INVERT_Y 1
// ====================================================================

static inline uint16_t clamp_u16(int32_t v, uint16_t lo, uint16_t hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return (uint16_t)v;
}

static uint16_t linear_interpolate(uint16_t raw, uint16_t min_raw, uint16_t max_raw,
                                   uint16_t min_lcd, uint16_t max_lcd) {
    if (raw < min_raw) raw = min_raw;
    if (raw > max_raw) raw = max_raw;
    int32_t raw_range = (int32_t)max_raw - (int32_t)min_raw;
    int32_t lcd_range = (int32_t)max_lcd - (int32_t)min_lcd;
    if (raw_range <= 0) return min_lcd;
    int32_t scaled = ((int32_t)raw - (int32_t)min_raw) * lcd_range / raw_range;
    return (uint16_t)(min_lcd + scaled);
}

static void sample_raw_point(struct TSPoint *p) {
    // Simple triple-sample median to reduce noise
    struct TSPoint a={0}, b={0}, c={0};
    getPoint(&a); sleep_us(300);
    getPoint(&b); sleep_us(300);
    getPoint(&c);

    // Median-by-hand (small N); good enough for lab
    #define MED3(u,v,w) (((u)>(v)) ? (((v)>(w))?(v):(((u)>(w))?(w):(u))) \
                                   : (((u)>(w))?(u):(((v)>(w))?(w):(v))))
    p->x = MED3(a.x, b.x, c.x);
    p->y = MED3(a.y, b.y, c.y);
    p->z = MED3(a.z, b.z, c.z);
    #undef MED3
}

bool get_ts_lcd(uint16_t *px, uint16_t *py) {
    struct TSPoint p = {0,0,0};
    sample_raw_point(&p);

    bool pressed;
#if Z_LOWER_IS_MORE_PRESSURE
    pressed = (p.z <= PRESSURE_THRESHOLD);
#else
    pressed = (p.z >= PRESSURE_THRESHOLD);
#endif
    if (!pressed) return false;

    // Map raw→lcd with optional swap/invert
    uint16_t rx = p.x, ry = p.y;
#if TS_SWAP_XY
    uint16_t tmp = rx; rx = ry; ry = tmp;
#endif
#if TS_INVERT_X
    rx = (uint16_t)(TS_MIN_X + TS_MAX_X - rx);
#endif
#if TS_INVERT_Y
    ry = (uint16_t)(TS_MIN_Y + TS_MAX_Y - ry);
#endif

    uint16_t lcd_x = linear_interpolate(rx, TS_MIN_X, TS_MAX_X, 0, ADAFRUIT_LCD_MAX_X);
    uint16_t lcd_y = linear_interpolate(ry, TS_MIN_Y, TS_MAX_Y, 0, ADAFRUIT_LCD_MAX_Y);

    // Final clamp
    *px = clamp_u16(lcd_x, 0, ADAFRUIT_LCD_MAX_X);
    *py = clamp_u16(lcd_y, 0, ADAFRUIT_LCD_MAX_Y);
    return true;
}

void ts_lcd_init() {
    adc_init();
    tft_init_hw();
    tft_begin();
    tft_setRotation(LCD_ROTATION);
    tft_fillScreen(ILI9340_BLACK);
}
