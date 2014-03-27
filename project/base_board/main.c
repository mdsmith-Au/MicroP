#include "arm_math.h"
#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "../common/motors_driver.h"

/*!
 @brief Program entry point
 */
int main (void) {
	init_motors();
	
	roll_move_to_angle(0);
	pitch_move_to_angle(0);
	
	roll_move_to_angle(45);
	pitch_move_to_angle(90);
	
	roll_move_to_angle(90);
	pitch_move_to_angle(45);
	
	roll_move_to_angle(0);
	pitch_move_to_angle(0);
	
	// The below doesn't really need to be in a loop
	while(1){
		osDelay(osWaitForever);
	}
}

