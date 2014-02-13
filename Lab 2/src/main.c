/**
 * @file main.c
 * ECSE 426 Lab 2 main entry point.
 * The main entry point is responsible for configuring and setting up 
 * any and all necessary hardware components for the temperature circuit 
 * to operate correctly.
 */

#include <stdio.h>
#include "adc_init.h"
#include "alarm.h"
#include "display_driver.h"
#include "gpio_init.h"
#include "temp_processing.h"

/** 
 * The interval at which the SysTick timer will generate interrupts.
 * Computed as 1/sampling_freq * system_core_clock, where...
 *   sampling_freq     = 25Hz
 *   system_core_clock = 168MHz
 */
#define TICK_DELAY 6720000 

void turnOnGreenLED(void);

/** 
 * Software flag set by the SysTick timer when an interrupt is generated 
 * at a fixed interval.
 * @c uint_fast16_t is used so the compiler uses the max speed implementation 
 * dependent on processor architecture. The interrupt service routine should 
 * be as fast as possible, hence the use of this "fast" type.
 */
static volatile uint_fast16_t ticks;

/**
 * Controls the rate of how often the LED 7-segment displays are updated.
 */
static volatile uint_fast16_t delay;

/**
 * Main entry point. Calls various external functions to set up the hardware
 * and controls the main time loop for the SysTick timer.
 */
int main() {
	GPIO_configure();                    // Set up GPIO
	ADC_configure();                     // Configure ADC (temp sensor @ ADC1_IN16)
	calibrateTempSensor();               // Calibrate temp sensor using factory data
	initFilterBuffer();                  // Ensure memory is clean in filter
	
	ticks = 0;
	delay = 0;
	SysTick_Config(TICK_DELAY);          // Set up SysTick timer to interrupt every TICK_DELAY
	
	PWM_configure();                     // Configure the Pulse Width Modulator
	
	while(1) {		
		// Loop until SysTick generates an interrupt
		while(!ticks) {
            // Output the current temperature readings to 7-segment displays
			draw();
            
            // Software delay between each draw to prevent flickering.
            // Value of 10000 determined to work by experimentation.
			int i = 0;
			while (i < 10000) i++;
		}
		
		// Reset the tick flag after interrupt is handled.
		ticks = 0;
		
		float temp = getAndAverageTemp();
        
		alarmCheckTemp(temp);
		printf("Temp: %f\n", temp);
		
        // Update the LED displays only if the delay time control has elapsed.
		if (delay == 0) {
			displayNum(temp);
		}
		
        // Increment the delay time control. 32 was determined by trial-and-error.
		delay = (delay + 1) % 32;
	}
}

/** 
 * Interrupt handler for the SysTick timer.
 * When SysTick generates an interrupt, set @c ticks to 1
 */
void SysTick_Handler() {
	ticks = 1;
}

/* Turns on the green LED light. For testing purposes only. */
void turnOnGreenLED() {
    GPIO_SetBits(GPIOD, GPIO_Pin_12);
}

