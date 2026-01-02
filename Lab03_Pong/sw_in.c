// Initializes input switches, reads button states, and provides functions for detecting player paddle presses during the game.
#include "sw_in.h"

static absolute_time_t last_press[32];

void sw_init(uint pin) {
    gpio_init(pin);
    gpio_set_dir(pin, false);
    gpio_pull_up(pin);
    last_press[pin] = get_absolute_time();
}

bool sw_pressed(uint pin) {
    if (!gpio_get(pin)) { // active low
        absolute_time_t now = get_absolute_time();
        if (absolute_time_diff_us(last_press[pin], now) > 200000) {
            last_press[pin] = now;
            return true;
        }
    }
    return false;
}
