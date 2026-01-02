// Implements the game’s finite state machine: manages serve, play, and miss states, tracks ball movement, and handles paddle inputs.

#include "fsm.h"
#include "led_out.h"
#include "sw_in.h"
#include <stdio.h>
#include <stdlib.h>

enum State { SERVE, PLAY, MISS };
static enum State state;
static int ball_pos;
static int dir;      // +1 right, -1 left
static int delay_ms; // ball speed
#define LEFT_BTN 16
#define RIGHT_BTN 17
#define LEFT_END 0
#define RIGHT_END 7

void fsm_init() {
    led_init();
    sw_init(LEFT_BTN);
    sw_init(RIGHT_BTN);
    srand(time_us_32());
    if (rand() & 1) {
        ball_pos = LEFT_END; dir = +1;
        printf("L serve\n");
    } else {
        ball_pos = RIGHT_END; dir = -1;
        printf("R serve\n");
    }
    led_set(ball_pos);
    state = SERVE;
    delay_ms = 300;
}

void fsm_update() {
    // added for debugging purposes through PuTTY.
    switch (state) {
        case SERVE:
            printf("Current state: SERVE\n");
            break;
        case PLAY:
            printf("Current state: PLAY\n");
            break;
        case MISS:
            printf("Current state: MISS\n");
            break;
    }
    
    switch (state) {
        case SERVE:
            if ((ball_pos==LEFT_END && sw_pressed(LEFT_BTN)) ||
                (ball_pos==RIGHT_END && sw_pressed(RIGHT_BTN))) {
                state = PLAY;
            }
            break;

        case PLAY:
            sleep_ms(delay_ms);
            ball_pos += dir;
            led_set(ball_pos);
            if ((ball_pos==LEFT_END && sw_pressed(LEFT_BTN)) ||
                (ball_pos==RIGHT_END && sw_pressed(RIGHT_BTN))) {
                dir = -dir; // reverse direction
                if (delay_ms > 100) delay_ms -= 20;
            } else if (ball_pos==LEFT_END || ball_pos==RIGHT_END) {
                state = MISS;
            }
            break;

        case MISS:
            int winner = (ball_pos==LEFT_END)? RIGHT_END : LEFT_END;
            led_flash(winner);
            printf("Player %s wins!\n", (winner==LEFT_END)?"L":"R");
            fsm_init(); // restart
            break;
    }
}
