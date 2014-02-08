#include <stdio.h>

#include "gpio_example.h"
#include "adc_init.h"
#include "temp_processing.h"

#define TICK_DELAY 6720000 /* 1/Freq * Core Clock , freq = 25Hz */

/* Variable that determines whether to process interrupt or not 
 * Defined as uint_fast16_t so compiler uses max. speed implementation 
 * dependent on processor architecture */
static volatile uint_fast16_t ticks;


int main()
{

	
	GPIO_example_config();  // Set up GPIO
	GPIO_SetBits(GPIOD, GPIO_Pin_12); // Light up green LED

	ADCConfig(); //Configure ADC (temp sensor @ ADC1_IN16 )
	
	calibrateTemp(); // Calibrate temp sensor using factory data
	
	initializeBuffer(); // Ensure memory is clean in filter
	
	ticks = 0;
	SysTick_Config( TICK_DELAY );
	
	while(1) {
		
		// Wait for interrupt for tick; don't do anything
		while(!ticks);
		
		//Great, interrupt! Reset interrupt ticks and do stuff
		ticks = 0;
		
		printf("Temp: %f\n", getAndAverageTemp());
	}
	
}

/* Handle Systick; should be 25Hz freq */
void SysTick_Handler() {
	// Let main while loop proceed
	ticks = 1;
}


