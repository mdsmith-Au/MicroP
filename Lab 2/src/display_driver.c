#include "display_driver.h"

// Numbers to be drawn on display
uint8_t first_GBL, second_GBL, third_GBL;

// Whose turn is it to display a number
uint8_t turn = 0;

/* Takes a float and prepares it for display.
 * Does not actually drive the display */
void displayNum(float number) {
	
	char string[3];
  
	// Conversion idea from Loki @ http://stackoverflow.com/a/339161
	// Convers the float to a string where we can extract individual digits
	sprintf(string, "%f", number);
	
	//Convert to integer
	const uint8_t first = convertToInt(string[0]);
	const uint8_t second = convertToInt(string[1]);
	const uint8_t third = convertToInt(string[2]);
	
	// Convert to pin setting
	first_GBL = convertToGPIO(first);
	second_GBL = convertToGPIO(second);
	third_GBL = convertToGPIO(third);
}


/* Sends a number previous given in displayNum() to the display */
void draw() {
	
	/* We set the display based on whose turn it is 
	 * First display is Pin 1, second is Pin 3, third is Pin 2
   * NoteL GPIO_Write not used because it would interfere with built-in LEDs	*/
	
	if (turn == 0) {
		
		// Clear all selection and segment lines
		GPIO_ResetBits(GPIOD, ALL_DISP);
		// Set selection line to display 1
		GPIO_SetBits(GPIOD, GPIO_Pin_1);
		// Output number to segment lines
		GPIO_SetBits(GPIOD, first_GBL);
		
		turn = 1;
	}
	else if (turn == 1) {

		GPIO_ResetBits(GPIOD, ALL_DISP);
		GPIO_SetBits(GPIOD, GPIO_Pin_3);
		GPIO_SetBits(GPIOD, second_GBL);
		
		turn = 2;
	}
	else if (turn == 2) {
		
		GPIO_ResetBits(GPIOD, ALL_DISP);
		GPIO_SetBits(GPIOD, GPIO_Pin_2);
		GPIO_SetBits(GPIOD, third_GBL);
		
		turn = 0;
	}
	
}

/* Converts a number into pin assignments using defined GPIO settings */
static int convertToGPIO(uint8_t num) {
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
static uint8_t convertToInt(char num) {
	// 0 -> 9 is 48 -> 57 in ASCII
	return (num-48);
}
