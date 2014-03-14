#include "arm_math.h"

#include "stm32f4xx.h"
#include "cmsis_os.h"
#include <stdio.h>

// Custom code includes
#include "adc_init.h"
#include "temp_processing.h"

/*!
 @brief Thread to perform menial tasks such as switching LEDs
 @param argument Unused
 */
void getTempThread(void const *argument);

FilterStruct tempFilterBuffer;

//! Thread structure for above thread
osThreadDef(getTempThread, osPriorityNormal, 1, 0);

/*!
 @brief Program entry point
 */
int main (void) {
	// ID for thread
	osThreadId tid_getTempThread;

  ADC_configure();
  calibrateTempSensor();
  initFilterBuffer(&tempFilterBuffer);

	// Start thread
  tid_getTempThread = osThreadCreate(osThread(getTempThread), NULL);
}


void getTempThread(void const *argument) {
  while (1) {
     printf("Temp: %f\n", getAndAverageTemp(&tempFilterBuffer));
     osDelay(500);
  }
}

/*
 Notes:
  For LCD:
  B0 = Reg select
  B1 = read/write
  B2 = enable
  B4 = data 0
  B5 = data 1
  B7 = data 2
  B8 = data 3
  B11 = data 4
  B12 = data 5
  B13 = data 6
  B14 = data 7

*/
