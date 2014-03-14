#ifndef _ACCELEROMETER_H
#define _ACCELEROMETER_H

/* Accelerometer driver
 * Note that #define USE_DEFAULT_TIMEOUT_CALLBACK  was uncommented in the driver
   per instructions included with it */
#include "stm32f4_discovery_lis302dl.h"
// For print statements
#include <stdio.h>

// Math libraries + lookup table version of the arctan function
#include "math.h"
#include "atan_LUT.h"

// SMA filter
#include "filter.h"

/* Calibration settings
 * Derived manually for this board using Appendix A
 * in ST Doc ID 17289 Rev 1 via MATLAB
 * Using floats because extreme accuracy is not the
 * most critical and they are faster than doubles */
#define ACC11 (float)1.003550281724753
#define ACC12 (float)-0.026368720829035
#define ACC13 (float)0.011792873245764
#define ACC21 (float)0.006759888036923
#define ACC22 (float)1.011703161339346
#define ACC23 (float)-0.011343179517597
#define ACC31 (float)0.006985319585343 
#define ACC32 (float)-0.029085274348594
#define ACC33 (float)0.989103078795187

#define ACC10 (float)-39.815373709157427
#define ACC20 (float)15.741238837260321
#define ACC30 (float)99.380766695848592

// Sets up the accelerometer to standard settings
void Accelerometer_configure(void);
// A debug method to calibrate the accelerometer and print raw data.  Do not
// use during normal runtime
void Accelerometer_calibrate(void);
// Return pitch and roll based on accelerometer x,y and z data in mg.
int Accelerometer_get_pitch(int x, int y, int z);
int Accelerometer_get_roll(int x, int y, int z);
// Writes x/y/z acceleration in mg to parameters
void Accelerometer_get_data(int* x, int* y, int* z);


#endif
