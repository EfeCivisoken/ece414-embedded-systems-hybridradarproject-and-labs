#ifndef MOTION_H
#define MOTION_H

#include <stdbool.h>
#include <stdint.h>

/*
 ======================================================================
    HB100 Doppler Motion Detection Module (Header)
    ---------------------------------------------------------------
    This module reads the analog IF output of HB100 radar modules
    through the Raspberry Pi Pico ADC and computes the RMS value
    of the AC component. Motion is detected when RMS exceeds a
    configured threshold (with hysteresis for stability).

    Public API:
        Motion_Init(left_pin, right_pin)
        Motion_Detected(left_or_right)
        Motion_GetLastRMS(left_or_right)
 ======================================================================
*/

/**
 * @brief Initialize motion detector ADC pins.
 *
 * Expected pins: GP26, GP27, GP28 → ADC0, ADC1, ADC2.
 *
 * @param adc_left_pin   GPIO number used for left HB100 IF output
 * @param adc_right_pin  GPIO number used for right HB100 IF output
 */
void Motion_Init(uint32_t adc_left_pin, uint32_t adc_right_pin);

/**
 * @brief Check whether motion is detected on the chosen radar.
 *
 * Performs ~220 ADC samples, computes RMS, applies hysteresis, and
 * returns a boolean motion/no-motion output.
 *
 * @param left  If true → use left HB100 module; false → right module
 * @return true if motion is detected, false otherwise
 */
bool Motion_Detected(bool left);

/**
 * @brief Get the last computed RMS value for debugging and display.
 *
 * @param left  true = left RMS, false = right RMS
 * @return      last RMS value (float)
 */
float Motion_GetLastRMS(bool left);

#endif // MOTION_H
