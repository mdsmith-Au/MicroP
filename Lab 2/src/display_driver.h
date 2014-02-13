#ifndef _DISPLAY_DRIVER_H_
#define _DISPLAY_DRIVER_H_

/**
 * @file display_driver.h
 * Header file for the 7 segment display driver.
 */

#include <stdio.h>
#include "gpio_init.h"

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
 
/* Pin mapping reference
 * - Pins 3, 4, 5 are used as select lines for the 
 *   first, second, and third displays respectively
 * - Segments and pin associations are as follows
 *
 *    Diagram:              Segment:         Pin:
 *     _____                Top Left         6
 *    | 10  |               Top              10
 *  6 |     | 11            Top Right        11
 *    |__7__|               Middle           7
 *    |     |               Bottom Left      8
 *  8 |     | 13  _         Bottom           13
 *    |_____|    |_|        Bottom Right     9
 *       9        12        Dot              12
 *
 */ 

/**
 * Prepares a number to send to one of the displays.
 * @param number the number to send
 */
void displayNum(float number);

/**
 * Selects which display to draw to and sets the appropriate segments.
 */
void draw(void);

#endif
