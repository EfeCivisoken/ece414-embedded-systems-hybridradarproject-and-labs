// Controls LED behavior: initializes pins, lights the active “ball” position, and flashes the winner’s LED after a missed return.
#include "led_out.h"

static const uint LED_PINS[] = {2,3,4,5,6,7,8,9}; // adjust to wiring
#define NUM_LEDS (sizeof(LED_PINS)/sizeof(LED_PINS[0]))

void led_init() {
    for (int i=0;i<NUM_LEDS;i++) {
        gpio_init(LED_PINS[i]);
        gpio_set_dir(LED_PINS[i], true);
    }
}

void led_set(uint8_t pos) {
    for (int i=0;i<NUM_LEDS;i++)
        gpio_put(LED_PINS[i], i==pos);
}

void led_flash(uint8_t pos) {
    for (int k=0;k<3;k++) {
        gpio_put(LED_PINS[pos], 1);
        sleep_ms(200);
        gpio_put(LED_PINS[pos], 0);
        sleep_ms(200);
    }
}
