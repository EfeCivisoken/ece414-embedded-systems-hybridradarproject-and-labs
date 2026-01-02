// Sets up standard I/O, initializes the FSM, and runs the continuous and very modularly designed game loop that drives Pong logic.
#include "pico/stdlib.h"
#include "fsm.h"

int main() {
    stdio_init_all();   // UART init
    fsm_init();
    while (1) {
        fsm_update();
    }
}
