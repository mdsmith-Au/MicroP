#include <stdio.h>
#include "gpio_example.h"

/* Bank displays are connected to */
#define GPIO_BANK GPIOE

/* Which display is on which pin */
#define DISPLAY_ONE GPIO_Pin_3
#define DISPLAY_TWO GPIO_Pin_4
#define DISPLAY_THREE GPIO_Pin_5

/* Segment to pin mapping */
#define TOP_LEFT GPIO_Pin_6
#define TOP GPIO_Pin_10
#define TOP_RIGHT GPIO_Pin_11
#define MIDDLE GPIO_Pin_7
#define BOTTOM_LEFT GPIO_Pin_8
#define BOTTOM_RIGHT GPIO_Pin_13
#define BOTTOM GPIO_Pin_9
#define DOT GPIO_Pin_12

/* Number to segment mapping */
#define ZERO (TOP_LEFT | TOP | TOP_RIGHT | BOTTOM_LEFT | BOTTOM_RIGHT | BOTTOM )
#define ONE (TOP_RIGHT | BOTTOM_RIGHT)
#define TWO (TOP | TOP_RIGHT | MIDDLE | BOTTOM_LEFT | BOTTOM )
#define THREE (TOP | TOP_RIGHT | MIDDLE | BOTTOM_RIGHT | BOTTOM )
#define FOUR (TOP_LEFT | MIDDLE | TOP_RIGHT | BOTTOM_RIGHT )
#define FIVE (TOP | TOP_LEFT | MIDDLE | BOTTOM_RIGHT | BOTTOM )
#define SIX (TOP | TOP_LEFT | MIDDLE | BOTTOM | BOTTOM_RIGHT | BOTTOM_LEFT )
#define SEVEN (TOP | TOP_RIGHT | BOTTOM_RIGHT )
#define EIGHT (TOP | TOP_LEFT | TOP_RIGHT | MIDDLE | BOTTOM_LEFT | BOTTOM_RIGHT | BOTTOM )
#define NINE (TOP | TOP_LEFT | TOP_RIGHT | MIDDLE | BOTTOM_RIGHT )



/* Select lines: Pins 3,4,5 for displays 1,2,3
 * Segment lines:

   10
	 _
6	|	| 11
	 _ 7
8	|	| 13
   _
   9
Dot : 12
		
		*/
void displayNum(float number);
void draw(void);
static uint16_t convertToGPIO(uint8_t num);
static uint16_t convertToInt(char num);
