// ================================
// MODULE: MAIN CONTROL LOOP
// ================================
#include "pico/stdlib.h"
#include <stdint.h>
#include <stdio.h>

#include "servo.h"
#include "ultrasonic.h"
#include "motion.h"
#include "buzzer.h"
#include "serial.h"
#include "display.h"
#include "ts_lcd.h"


// ------------------ Pin map (clean, no TFT conflicts) ------------------

// Servos (PWM-capable)
#define SERVO_LEFT     6     // PWM2A
#define SERVO_RIGHT    7     // PWM2B
//

// Ultrasonic sensors
#define TRIG_LEFT      10
#define ECHO_LEFT      11

#define TRIG_RIGHT     12
#define ECHO_RIGHT     13

// PIR motion sensors
// changed it to 26 and 27 given they need different channels
// due to analogue output
#define MOTION_LEFT    26                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
#define MOTION_RIGHT   27 

// Buzzers
#define BUZZER_LEFT    21
#define BUZZER_RIGHT   22


int main() {
    stdio_init_all();
    sleep_ms(200);   // give USB and power rails a moment to settle

    // ---------------- DISPLAY + TOUCH ----------------
    // ts_lcd_init() does ALL low-level TFT init via TFTMaster
    ts_lcd_init();          
    Display_Init();         // radar drawing state (no TFT hw init)
    Display_DrawBackground();
    Serial_Init();

    // ---------------- SENSORS / ACTUATORS ------------
    Servo_Init(SERVO_LEFT, SERVO_RIGHT);
    Ultrasonic_Init(TRIG_LEFT, ECHO_LEFT, TRIG_RIGHT, ECHO_RIGHT);
    
    Motion_Init(MOTION_LEFT, MOTION_RIGHT);
    Buzzer_Init(BUZZER_LEFT, BUZZER_RIGHT);

    // start from left sensor
    bool current_left = true;
    Ultrasonic_Trigger(current_left);

    absolute_time_t last_servo_update = get_absolute_time();

    while (true) {
        // ==============================================
        // TOUCHSCREEN (Debounced)
        // ==============================================
        uint16_t tx, ty;
        if (get_ts_lcd(&tx, &ty)) {
            // later: pause / clear / mode switching with UI buttons
            sleep_ms(150);   // simple debounce
        }

        // ==============================================
        // SERVO MOTION (about 20 Hz)
        // ==============================================
        if (absolute_time_diff_us(last_servo_update, get_absolute_time()) > 20000) {
            Servo_UpdateSweep();           // updates both servos internally
            last_servo_update = get_absolute_time();
        }

        // ==============================================
        // ULTRASONIC COMPLETION
        // ==============================================
        if (!Ultrasonic_IsBusy()) {
            // The last triggered sensor (current_left) has a new reading
            float angle    = Servo_GetAngle(current_left);
            float distance = Ultrasonic_GetDistance();
            bool  motion   = Motion_Detected(current_left);

            // radar heatmap + blip drawing
            Display_Update(current_left, angle, distance, motion);

            // log to USB serial for debugging
            Serial_SendReading(current_left, angle, distance, motion);

            // buzzer: only one side gets a valid distance at a time
            float dist_l = current_left ? distance : -1.0f;
            float dist_r = current_left ? -1.0f    : distance;
            Buzzer_Update(dist_l, dist_r);

            // switch to the other sensor, wait a bit to avoid cross-talk
            current_left = !current_left;
            sleep_ms(30);
            Ultrasonic_Trigger(current_left);
        }

         // ==============================================
        // DEBUG: RAW ECHO PIN LEVELS
        // ==============================================
        printf("RAW ECHO: L=%d  R=%d\r\n",
            gpio_get(ECHO_LEFT),
            gpio_get(ECHO_RIGHT));
        sleep_ms(30);   // do not remove - avoids USB flooding
     
     
        // small delay so the loop doesn’t thrash
        sleep_ms(5);
        
    }

    return 0;
}
