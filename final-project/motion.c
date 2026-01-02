#include "motion.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"
#include <math.h>

#define SAMPLE_COUNT      220        // number of ADC samples per check
#define MOTION_THRESHOLD  50.0f      // adjust depending on amplifier gain
#define HYSTERESIS        10.0f      // prevents rapid toggling

// ADC channels for left/right HB100 IF amplifiers
static uint32_t adc_ch_left;
static uint32_t adc_ch_right;

// Store last RMS values for debugging and display
static float last_rms_left  = 0.0f;
static float last_rms_right = 0.0f;

// Public getter for debugging
float Motion_GetLastRMS(bool left) {
    return left ? last_rms_left : last_rms_right;
}

// ------------------------------------------------------
// Initialize: configure ADC pins for HB100 IF channels
// ------------------------------------------------------
void Motion_Init(uint32_t adc_left_pin, uint32_t adc_right_pin) {
    adc_init();

    // Configure GPIOs for analog input
    adc_gpio_init(adc_left_pin);
    adc_gpio_init(adc_right_pin);

    // Pico mapping: GP26→ADC0, GP27→ADC1, GP28→ADC2
    adc_ch_left  = adc_left_pin  - 26;
    adc_ch_right = adc_right_pin - 26;
}

// ------------------------------------------------------
// Internal: compute RMS of AC component of HB100 IF signal
// ------------------------------------------------------
static float compute_rms(uint32_t channel) {
    adc_select_input(channel);

    uint64_t sum  = 0;
    uint64_t sum2 = 0;

    for (int i = 0; i < SAMPLE_COUNT; i++) {
        uint16_t raw = adc_read();    // 0–4095 (12-bit)
        sum  += raw;
        sum2 += (uint64_t)raw * raw;
        sleep_us(150);                // ~6.6 kHz sampling → good for 10–200 Hz Doppler
    }

    float mean  = (float)sum  / SAMPLE_COUNT;
    float mean2 = (float)sum2 / SAMPLE_COUNT;

    float variance = mean2 - (mean * mean);
    if (variance < 0) variance = 0.0f;

    return sqrtf(variance);
}

// ------------------------------------------------------
// Final API: detect motion using RMS + hysteresis
// ------------------------------------------------------
bool Motion_Detected(bool left) {
    uint32_t channel = left ? adc_ch_left : adc_ch_right;

    // Compute RMS for this check
    float rms = compute_rms(channel);

    // Store for debugging
    if (left) last_rms_left = rms;
    else      last_rms_right = rms;

    // Independent state machines (left/right)
    static bool state_left  = false;
    static bool state_right = false;

    bool *state = left ? &state_left : &state_right;

    // Hysteresis logic
    if (!*state) {
        if (rms > MOTION_THRESHOLD)
            *state = true;
    } else {
        if (rms < (MOTION_THRESHOLD - HYSTERESIS))
            *state = false;
    }

    return *state;
}
