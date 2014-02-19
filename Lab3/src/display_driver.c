/**
 * @file display_driver.c
 * Display driver implementation.
 * This file mainly handles the conversion between real numbers 
 * and the selection of which segments to switch on for the display 
 * to show a correct representation of that number.
 */

#include "display_driver.h"

static uint16_t convertToGPIO(uint8_t num);
static uint16_t convertToInt(char num);

// Numbers to be drawn on display
uint16_t first_GBL, second_GBL, third_GBL;

// Tracks which 7 segment display to draw to
uint8_t turn = 0;

/* Takes a float and prepares it for display.
 * Does not actually drive the display */
void displayNum(float number) {
	char string[4];
  
	/* Converts the float to a string where we can extract individual digits
	 * .1 specifies only 1 digit after decimal
	 * Output ex. : 38.1 
	 * Note:  sprintf automatically rounds */
	sprintf(string, "%.1f", number);
	
    // Decimal point @ 2 is not an integer
	uint8_t first    = convertToInt(string[0]);
	uint8_t second   = convertToInt(string[1]);
	uint8_t third    = convertToInt(string[3]);
	
	// Convert to pin setting	
	first_GBL        = convertToGPIO(first);
	second_GBL       = convertToGPIO(second);
	third_GBL        = convertToGPIO(third);
	
	/* @TODO: Why when using optimization does the code freeze at the line below (literally at the bracket)? */
}

/* Sends a number previously given in displayNum() to the display */
void draw() {
	if (turn == 0) {
		// Write number to display lines along with appropriate display select
		GPIO_Write(GPIO_BANK, first_GBL | DISPLAY_ONE);
	}
	else if (turn == 1) {
    // In the case of the second display, we also show the dot
		GPIO_Write(GPIO_BANK, second_GBL | DISPLAY_TWO | DOT);
	}
	else if (turn == 2) {
		GPIO_Write(GPIO_BANK, third_GBL | DISPLAY_THREE);
	}
	
	// Set for next display on next execution
	turn = (turn + 1) % 3;
}

/**
 * Converts a number into pin assignments using defined GPIO settings
 * @param num the number
 * @return the pin assignments to display that number, as an integer
 */
static uint16_t convertToGPIO(uint8_t num) {
	if (num == 0) {
		return ZERO;
	}
	else if (num == 1) {
		return ONE;
	}
	else if (num == 2) {
		return TWO;
	}
	else if (num == 3) {
		return THREE;
	}
	else if (num == 4) {
		return FOUR;
	}
	else if (num == 5) {
		return FIVE;
	}
	else if (num == 6) {
		return SIX;
	}
	else if (num == 7) {
		return SEVEN;
	}
	else if (num == 8) {
		return EIGHT;
	}
	else if (num == 9) {
		return NINE;
	}
	return ZERO;         // Invalid number, just set to zero
}

/**
 * Returns 8-bit integer representation of a character
 * if the char is between 0 and 9 only. 
 * Can simply subtract 48 from the char due to ASCII ordering
 * (char '0' has ASCII code 48, char '1' has ASCII code 49, etc...)
 * @param num a numeric character
 * @return the integer equivalent of @var num
 */
static uint16_t convertToInt(char num) {
	return num - 48;
}
