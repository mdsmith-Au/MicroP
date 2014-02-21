#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "math.h"
#include "atan_LUT.h"

#include "motor.h"
#include "gpio_init.h"
#include "accelerometer.h"

/** 
 * The interval at which the SysTick timer will generate interrupts.
 * Computed as 1/sampling_freq * system_core_clock, where...
 *   sampling_freq     = 100Hz
 *   system_core_clock = 168MHz
 */
#define TICK_DELAY 1680000

/** 
 * Software flag set by the SysTick timer when an interrupt is generated 
 * at a fixed interval.
 * @c uint_fast16_t is used so the compiler uses the max speed implementation 
 * dependent on processor architecture. The interrupt service routine should 
 * be as fast as possible, hence the use of this "fast" type.
 */
static volatile uint_fast16_t ticks;

int main()
{
	GPIO_configure();
	PWM_configure();
	
	uint8_t test = 0;
  
  ticks = 0;
	SysTick_Config(TICK_DELAY);    
	
	Accelerometer_configure();
  
	while(1){
    
    /*int buffer[3];
    LIS302DL_ReadACC(buffer);
    
    for (int i = 0; i < sizeof(buffer)/sizeof(int); i++) {
      printf("buf[%i] = %i\n", i, buffer[i]);
    }*/
    /*
		if (test == 0)
		{
			TIM1 -> CCR4 = 1500;
			test = 1;
		}
		else if (test == 1)
		{
			TIM1 -> CCR4 = 1100;
			test = 2;
	}
	else if (test == 2){
		
		TIM1 -> CCR4 = 1500+(2*400);
		test = 0;
	}*/
  
  while(!ticks);
		
		// Reset the tick flag after interrupt is handled.
		ticks = 0;
		

  
  
  }
}


/** 
 * Interrupt handler for the SysTick timer.
 * When SysTick generates an interrupt, set @c ticks to 1
 */
void SysTick_Handler() {
	ticks = 1;
}







