#include "display_driver.h"

// Numbers to be drawn on display
uint16_t first_GBL, second_GBL, third_GBL;
uint16_t old_first_GBL, old_second_GBL, old_third_GBL;

// Whose turn is it to display a number
uint8_t turn = 0;

/* Takes a float and prepares it for display.
 * Does not actually drive the display */
void displayNum(float number) {
	
	char string[4];
  
	/* Convers the float to a string where we can extract individual digits
	 * .1 specifies only 1 digit after decimal
	 * Output ex. : 38.1 
	 * Note:  sprintf automatically rounds */
	sprintf(string, "%.1f", number);
	
	//Convert to integer
	uint8_t first = convertToInt(string[0]);
	uint8_t second = convertToInt(string[1]);
	// Decimal point @ 2 is not an integer
	uint8_t third = convertToInt(string[3]);
	
	// Convert to pin setting
	old_first_GBL = first_GBL;
	old_second_GBL = second_GBL;
	old_third_GBL = third_GBL;
	
	first_GBL = convertToGPIO(first);
	second_GBL = convertToGPIO(second);
	third_GBL = convertToGPIO(third);
	
	/* TODO: Why when using optimization does the code freeze at the line below (literally at the bracket)? */
}


/* Sends a number previous given in displayNum() to the display */
void draw() {
	
	if (turn == 0) {
		// Write number to display lines along with appropriate display select
		GPIO_Write(GPIO_BANK, first_GBL | DISPLAY_ONE);
	}
	else if (turn == 1) {
		GPIO_Write(GPIO_BANK, second_GBL | DISPLAY_TWO | DOT);
	}
	
	else if (turn == 2) {
		GPIO_Write(GPIO_BANK, third_GBL | DISPLAY_THREE);
	}
	
	// Set for next display on next execution
	turn = (turn + 1) % 3;
}

/* Converts a number into pin assignments using defined GPIO settings */
static uint16_t convertToGPIO(uint8_t num) {
	if (num == 0) {
		return ZERO;
	}
	else if (num == 1){
		return ONE;
	}
	else if (num == 2){
		return TWO;
	}
	else if (num == 3){
		return THREE;
	}
	else if (num == 4){
		return FOUR;
	}
	else if (num == 5){
		return FIVE;
	}
	else if (num == 6){
		return SIX;
	}
	else if (num == 7){
		return SEVEN;
	}
	else if (num == 8){
		return EIGHT;
	}
	else if (num == 9){
		return NINE;
	}
	// Invalid number, just set to zero
	return ZERO;
}

/* Returns 8-bit integer representation of a character
 * if the char is between 0 and 9 only.  Otherwise,
 * it will produce garbage values*/
static uint16_t convertToInt(char num) {
	// 0 -> 9 is 48 -> 57 in ASCII
	return (num-48);
}
