// servo.c
#include "servo.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <stdbool.h>
#include <stdint.h>


static uint left_gpio, right_gpio;
static uint left_slice, right_slice;
static uint left_chan,  right_chan;

static float left_angle  = 90.0f;
static float right_angle = 90.0f;
static bool left_forward  = true;
static bool right_forward = false;

static inline uint16_t angle_to_us(float deg) {
    if (deg < 0.0f)   deg = 0.0f;
    if (deg > 180.0f) deg = 180.0f;
    return (uint16_t)(500.0f + (deg * 2000.0f / 180.0f));
}

static inline void set_servo_level(uint slice, uint chan, uint16_t us) {
    if (us < 500)  us = 500;
    if (us > 2500) us = 2500;
    pwm_set_chan_level(slice, chan, us);
}

void Servo_Init(uint32_t gpio_l, uint32_t gpio_r) {
    left_gpio  = gpio_l;
    right_gpio = gpio_r;

    gpio_set_function(left_gpio, GPIO_FUNC_PWM);
    gpio_set_function(right_gpio, GPIO_FUNC_PWM);

    left_slice  = pwm_gpio_to_slice_num(left_gpio);
    right_slice = pwm_gpio_to_slice_num(right_gpio);
    left_chan   = pwm_gpio_to_channel(left_gpio);
    right_chan  = pwm_gpio_to_channel(right_gpio);

    pwm_set_clkdiv(left_slice, 125.0f);
    pwm_set_clkdiv(right_slice, 125.0f);
    pwm_set_wrap(left_slice,  20000 - 1);
    pwm_set_wrap(right_slice, 20000 - 1);

    // start one at ~0°, the other at ~180° so they “fan” outward
    set_servo_level(left_slice,  left_chan,  angle_to_us(left_angle));
    set_servo_level(right_slice, right_chan, angle_to_us(right_angle));

    pwm_set_enabled(left_slice,  true);
    pwm_set_enabled(right_slice, true);
     // STOP SERVOS FOR 5 SECONDS TO SEE HALFWAY
     sleep_ms(5000);
}

void Servo_UpdateSweep(void) {
    const float STEP = 6.0f;     // increase this for more speed

    // left servo sweep
    if (left_forward)  left_angle  += STEP;
    else               left_angle  -= STEP;

    if (left_angle >= 180.0f) { left_angle = 180.0f; left_forward = false; }
    if (left_angle <=   0.0f) { left_angle =   0.0f; left_forward = true;  }

    set_servo_level(left_slice, left_chan, angle_to_us(left_angle));

    // right servo sweep
    if (right_forward) right_angle += STEP;
    else               right_angle -= STEP;

    if (right_angle >= 180.0f) { right_angle = 180.0f; right_forward = false; }
    if (right_angle <=   0.0f) { right_angle =   0.0f; right_forward = true;  }

    set_servo_level(right_slice, right_chan, angle_to_us(right_angle));
}


float Servo_GetAngle(bool left) {
    return left ? left_angle : right_angle;
}
