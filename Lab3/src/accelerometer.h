#ifndef _ACCELEROMETER_H
#define _ACCELEROMETER_H

// Accelerometer driver
#include "stm32f4_discovery_lis302dl.h"
// printf
#include <stdio.h>

// Math + lookup table arctan
#include "math.h"
#include "atan_LUT.h"

// SMA filter
#include "filter.h"

void Accelerometer_configure(void);
void Accelerometer_calibrate(void);
int Accelerometer_get_pitch(int x, int y, int z);
int Accelerometer_get_roll(int x, int y, int z);
void Accelerometer_get_data(int* x, int* y, int* z);


#endif
