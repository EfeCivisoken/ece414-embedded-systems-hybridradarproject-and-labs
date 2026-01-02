#pragma once
#include <stdbool.h>

// Initializes SW1 on GPIO16 and SW2 on GPIO17 as inputs with pull-ups.
void sw_in_init(void);

// Return true iff the corresponding switch is PRESSED.
// (active-low due to pull-ups)
bool sw_in_read1(void); // SW1 @ GPIO16
bool sw_in_read2(void); // SW2 @ GPIO17
