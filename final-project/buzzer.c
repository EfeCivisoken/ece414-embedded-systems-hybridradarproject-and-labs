
#include "buzzer.h"
#include "pico/stdlib.h"

static uint32_t buzz_l, buzz_r;

void Buzzer_Init(uint32_t pin_left, uint32_t pin_right) {
    buzz_l = pin_left;
    buzz_r = pin_right;
    gpio_init(buzz_l); gpio_set_dir(buzz_l, GPIO_OUT); gpio_put(buzz_l, 0);
    gpio_init(buzz_r); gpio_set_dir(buzz_r, GPIO_OUT); gpio_put(buzz_r, 0);
}

void Buzzer_Update(float dist_left, float dist_right) {
    gpio_put(buzz_l, (dist_left > 0 && dist_left <= 20.0f));
    gpio_put(buzz_r, (dist_right > 0 && dist_right <= 20.0f));
}