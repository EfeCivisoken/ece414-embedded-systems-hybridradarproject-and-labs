#ifndef SERIAL_H
#define SERIAL_H

#include <stdbool.h>

// Required to access RMS
#include "motion.h"

void Serial_Init();
void Serial_SendReading(bool left, float angle, float distance, bool motion);

#endif
