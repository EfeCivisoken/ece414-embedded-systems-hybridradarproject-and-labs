#include "pico/stdlib.h"
#include "led_out.h"

#define LED_FIRST 2
#define LED_COUNT 8

void led_out_init(void) {
    for (int i = 0; i < LED_COUNT; ++i) {
        const uint pin = LED_FIRST + i;
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_OUT);
        gpio_put(pin, 0);   // start off
    }
}

void led_out_write(uint8_t value) {
    // bit0 -> GPIO2 ... bit7 -> GPIO9
    for (int i = 0; i < LED_COUNT; ++i) {
        const uint pin = LED_FIRST + i;
        gpio_put(pin, (value >> i) & 0x1);
    }
}
