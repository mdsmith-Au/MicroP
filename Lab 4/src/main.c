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
void thread(void const * argument);
void getTempThread(void const *argument);

FilterStruct tempFilterBuffer;

//! Thread structure for above thread
osThreadDef(thread, osPriorityNormal, 1, 0);
osThreadDef(getTempThread, osPriorityNormal, 1, 0);

/*!
 @brief Program entry point
 */
int main (void) {
	// ID for thread
	osThreadId tid_thread1, tid_getTempThread;
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  ADC_configure();
  calibrateTempSensor();
  initFilterBuffer(&tempFilterBuffer);
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	// Start thread
	tid_thread1 = osThreadCreate(osThread(thread), NULL);
  tid_getTempThread = osThreadCreate(osThread(getTempThread), NULL);
}

void thread (void const *argument) {
	while(1){
		osDelay(1000);
		GPIOD->BSRRL = GPIO_Pin_12;
		osDelay(1000);
		GPIOD->BSRRH = GPIO_Pin_12;
	}
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
