#pragma once

// LED lane configuration
#define LED_COUNT 8
static const int LED_PINS[LED_COUNT] = {2,3,4,5,6,7,8,9};

// Button pins (active-low)
#define BTN_LEFT_PIN   14
#define BTN_RIGHT_PIN  15

// UART baud rate
#define UART_BAUD 115200

// Ball timing
#define START_DELAY_MS 300
#define MIN_DELAY_MS   100
#define SPEED_STEP_MS   25

// Flash timing
#define FLASH_COUNT      3
#define FLASH_ON_MS    120
#define FLASH_OFF_MS   120

// Debounce settings
#define DEBOUNCE_SAMPLE_MS  5
#define DEBOUNCE_STABLE_CNT 6
