// ultrasonic.c
#include "ultrasonic.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include <stdint.h>


#define SOUND_SPEED_CM_PER_US 0.0343f     // 343 m/s
#define ECHO_TIMEOUT_US       30000       // 30 ms

static uint32_t trig_l, echo_l, trig_r, echo_r;

static absolute_time_t echo_start_l, echo_end_l;
static absolute_time_t echo_start_r, echo_end_r;
static absolute_time_t trigger_time;

static volatile bool echo_done_l = true;
static volatile bool echo_done_r = true;

static bool is_left = true;

// ---------- IRQ callback ----------
static void echo_callback(uint gpio, uint32_t events) {
    if (gpio == echo_l) {
        if (events & GPIO_IRQ_EDGE_RISE) {
            echo_start_l = get_absolute_time();
        }
        if (events & GPIO_IRQ_EDGE_FALL) {
            echo_end_l = get_absolute_time();
            echo_done_l = true;
        }
    } else if (gpio == echo_r) {
        if (events & GPIO_IRQ_EDGE_RISE) {
            echo_start_r = get_absolute_time();
        }
        if (events & GPIO_IRQ_EDGE_FALL) {
            echo_end_r = get_absolute_time();
            echo_done_r = true;
        }
    }
}

// ---------- public API ----------
void Ultrasonic_Init(uint32_t t_l, uint32_t e_l,
                     uint32_t t_r, uint32_t e_r) {
    trig_l = t_l;  echo_l = e_l;
    trig_r = t_r;  echo_r = e_r;

    gpio_init(trig_l);
    gpio_set_dir(trig_l, GPIO_OUT);
    gpio_put(trig_l, 0);

    gpio_init(trig_r);
    gpio_set_dir(trig_r, GPIO_OUT);
    gpio_put(trig_r, 0);

    gpio_init(echo_l);
    gpio_set_dir(echo_l, GPIO_IN);
    gpio_pull_down(echo_l);   // <-- NEW, forces idle LOW

    gpio_init(echo_r);
    gpio_set_dir(echo_r, GPIO_IN);
    gpio_pull_down(echo_r);   // <-- NEW, forces idle LOW

    // register callback on echo_l, then enable on both
    gpio_set_irq_enabled_with_callback(
        echo_l,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
        true,
        &echo_callback
    );
    gpio_set_irq_enabled(
        echo_r,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
        true
    );
}

void Ultrasonic_Trigger(bool left) {
    is_left = left;
    trigger_time = get_absolute_time();

    if (left) {
        echo_done_l = false;
        echo_start_l = trigger_time;
        echo_end_l   = trigger_time;

        gpio_put(trig_l, 1);
        sleep_us(10);
        gpio_put(trig_l, 0);
    } else {
        echo_done_r = false;
        echo_start_r = trigger_time;
        echo_end_r   = trigger_time;

        gpio_put(trig_r, 1);
        sleep_us(10);
        gpio_put(trig_r, 0);
    }
}

// Returns true while waiting for echo, false when measurement finished/timeout.
bool Ultrasonic_IsBusy(void) {
    bool done = is_left ? echo_done_l : echo_done_r;

    if (!done) {
        int64_t dt = absolute_time_diff_us(trigger_time, get_absolute_time());
        if (dt > ECHO_TIMEOUT_US) {
            // timeout → mark as done with zero distance
            if (is_left) {
                echo_done_l = true;
                echo_end_l  = echo_start_l;
            } else {
                echo_done_r = true;
                echo_end_r  = echo_start_r;
            }
            return false;
        }
        return true; // still waiting
    }

    return false; // already done
}

float Ultrasonic_GetDistance(void) {
    absolute_time_t start = is_left ? echo_start_l : echo_start_r;
    absolute_time_t end   = is_left ? echo_end_l   : echo_end_r;

    int64_t us = absolute_time_diff_us(start, end);
    if (us <= 0) return 0.0f;

    return (float)us * SOUND_SPEED_CM_PER_US * 0.5f;
}
