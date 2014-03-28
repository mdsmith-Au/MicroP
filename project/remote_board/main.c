#include "arm_math.h"
#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "LCD_driver.h"

/*!
 @brief Program entry point
 */
int main (void) {
	
	osDelay(2000);
	LCD_configure();
	osDelay(5);
	printLCDString("Kevin did it!",1);
	// The below doesn't really need to be in a loop
	while(1){
		osDelay(osWaitForever);
	}
}
