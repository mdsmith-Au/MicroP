/**
 * @file main.c
 * ECSE 426 Lab 2 main entry point.
 * The main entry point is responsible for configuring and setting up 
 * any and all necessary hardware components for the temperature circuit 
 * to operate correctly.
 */

#include <stdio.h>
#include "adc_init.h"
#include "display_driver.h"
#include "gpio_example.h"
#include "temp_processing.h"

/** 
 * The interval at which the SysTick timer will generate interrupts.
 * Computed as 1/freq * Core_Clock, where freq = 25Hz, Core_Clock = 168MHz.
 */
#define TICK_DELAY 6720000 
//#define TICK_DELAY 2240000 

/** 
 * Software flag set by the SysTick timer when an interrupt is generated 
 * at a fixed interval.
 * @c uint_fast16_t is used so the compiler uses the max speed implementation 
 * dependent on processor architecture 
 */
static volatile uint_fast16_t ticks;

/**
 * Main entry point. Calls various external functions to set up the hardware
 * and controls the main time loop for the SysTick timer.
 * @returns 0 on successful exit, 1 otherwise.
 */
int main() {	
	GPIO_example_config();               // Set up GPIO
	GPIO_SetBits(GPIOD, GPIO_Pin_12);    // Light up green LED
	ADCConfig();                         // Configure ADC (temp sensor @ ADC1_IN16)
	calibrateTemp();                     // Calibrate temp sensor using factory data
	initializeBuffer();                  // Ensure memory is clean in filter
	
	
	ticks = 0;
	SysTick_Config(TICK_DELAY);
	
	while(1) {		
		// Wait for interrupt for tick; don't do anything
		while(!ticks) {
			draw();
			int i = 0;
			while (i < 10000) i++;
		}
		
		
		//Great, interrupt! Reset interrupt ticks and do stuff
		ticks = 0;
		
		
		float temp = getAndAverageTemp();
		printf("Temp: %f\n", temp);
		displayNum(temp);
	}
}

/** 
 * Interrupt handler for the SysTick timer.
 * When SysTick generates an interrupt, set @c ticks to 1
 */
void SysTick_Handler() {
	ticks = 1;
}


