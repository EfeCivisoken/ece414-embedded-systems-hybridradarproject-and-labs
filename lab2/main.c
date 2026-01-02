#include "pico/stdlib.h"
#include <stdint.h>
#include <stdbool.h>

#include "led_out.h"
#include "sw_in.h"

int main() {
    stdio_init_all();        // optional (USB prints if you add them)
    led_out_init();
    sw_in_init();

    while (true) {
        const bool s1 = sw_in_read1();  // SW1 on GPIO16
        const bool s2 = sw_in_read2();  // SW2 on GPIO17
        uint8_t pattern;

        if (s1 && s2)       pattern = 0xFF;  // both pressed → all on
        else if (s1)        pattern = 0x80;  // only SW1 → leftmost LED (bit7)
        else if (s2)        pattern = 0x01;  // only SW2 → rightmost LED (bit0)
        else                pattern = 0x00;  // none → all off

        led_out_write(pattern);

        // small delay helps with mechanical bounce and reduces CPU churn
        sleep_ms(10);
    }
}
