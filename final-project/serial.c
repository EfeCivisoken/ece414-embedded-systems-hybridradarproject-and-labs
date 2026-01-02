#include "serial.h"
#include "motion.h"
#include "pico/stdlib.h"
#include <stdio.h>

void Serial_Init() {
    stdio_init_all();
}

void Serial_SendReading(bool left, float angle, float distance, bool motion) {
    const char *side = left ? "L" : "R";
    float rms = Motion_GetLastRMS(left);  // <-- NEW: RMS for debugging

    if (distance <= 0.0f) {
        // Likely timeout or no echo
        printf("[%s] angle=%.1f deg  distance=NO_ECHO  motion=%d  rms=%.1f\r\n",
               side, angle, motion ? 1 : 0, rms);
    } else {
        printf("[%s] angle=%.1f deg  distance=%.1f cm  motion=%d  rms=%.1f\r\n",
               side, angle, distance, motion ? 1 : 0, rms);
    }

    // OPTIONAL: JSON format if needed later
    // printf("{\"side\":\"%s\",\"angle\":%.1f,\"distance\":%.2f,\"motion\":%d,\"rms\":%.1f}\n",
    //        side, angle, distance, motion ? 1 : 0, rms);
}
