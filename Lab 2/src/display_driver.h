#include <stdio.h>
#include "gpio_example.h"

/* Definitions for what pins map to what numbers */
#define ZERO (GPIO_Pin_11 | GPIO_Pin_8 |  GPIO_Pin_6 | GPIO_Pin_7 |  GPIO_Pin_10 |  GPIO_Pin_5)
#define ONE (GPIO_Pin_10 | GPIO_Pin_5)
#define TWO (GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_4 | GPIO_Pin_8 | GPIO_Pin_11)
#define THREE (GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_11)
#define FOUR (GPIO_Pin_6 | GPIO_Pin_4 | GPIO_Pin_10 | GPIO_Pin_5)
#define FIVE (GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_11)
#define SIX (GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_5 | GPIO_Pin_4)
#define SEVEN (GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_5)
#define EIGHT (GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_4 | GPIO_Pin_11 | GPIO_Pin_5 | GPIO_Pin_10)
#define NINE (GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_4 | GPIO_Pin_10 | GPIO_Pin_5)
#define DOT (GPIO_Pin_9)
#define ALL_DISP (GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11)

/*
	NES-3015BH pins, mapped to GPIOD:
	 7
	 -
 6|	|10
	 - 4
 8| |5
	 -
	 11
	  .
		9 
		
Select lines are 1 for first display, 3 for 2nd, 2 for third
Rest of the pins on GPIOD are not used for display*/
		
void displayNum(float number);
void draw(void);
static int convertToGPIO(uint8_t num);
static uint8_t convertToInt(char num);
