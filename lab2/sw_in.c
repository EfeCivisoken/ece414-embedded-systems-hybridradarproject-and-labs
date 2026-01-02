#include "pico/stdlib.h"
#include "sw_in.h"

#define SW1_PIN 16
#define SW2_PIN 17

void sw_in_init(void) {
    gpio_init(SW1_PIN);
    gpio_set_dir(SW1_PIN, GPIO_IN);
    gpio_pull_up(SW1_PIN);

    gpio_init(SW2_PIN);
    gpio_set_dir(SW2_PIN, GPIO_IN);
    gpio_pull_up(SW2_PIN);
}

// With pull-ups enabled: idle=1, pressed=0 basically inverts the read.
bool sw_in_read1(void) { return !gpio_get(SW1_PIN); }
bool sw_in_read2(void) { return !gpio_get(SW2_PIN); }
