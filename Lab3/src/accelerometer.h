#ifndef _ACCELEROMETER_H
#define _ACCELEROMETER_H

// Accelerometer driver
#include "stm32f4_discovery_lis302dl.h"
// printf
#include <stdio.h>

void Accelerometer_configure(void);
void Accelerometer_calibrate(void);
#endif
