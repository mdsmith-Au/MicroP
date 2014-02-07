#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include "gpio_example.h"
#include "adc_init.h"
#include "get_temp.h"
#include "filter.h"

#define TICK_DELAY 6720000 /* 1/Freq * Core Clock , freq = 25Hz */

static volatile uint_fast16_t ticks;


int main()
{

	
	GPIO_example_config();  // Set up GPIO
	GPIO_SetBits(GPIOD, GPIO_Pin_12); // Light up green LED

	ADC_Config(); //Configure ADC (temp sensor @ ADC1_IN16 )
	
	
	calibrateTemp(); // Calibrate temp sensor using factory data
	
	ticks = 0;
	SysTick_Config( TICK_DELAY );
	
	while(1) {
		
		// Wait for interrupt for tick; don't do anything
		while(!ticks);
		
		//Great, interrupt! Reset interrupt ticks and do stuff
		ticks = 0;
		
		//printf("Temp: %u\n", getTemp());
	}
	
}

/* Handle Systick; should be 25Hz freq */
void SysTick_Handler() {
	// Let main while loop proceed
	ticks = 1;
}


/*
	*	Configure ADC
	* Clock gating
	* Get temp via printf
	* max #  bits
	* data alignment: to the right
	* use independent mode
	* systick config
	* */


