#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "math.h"
#include "atan_LUT.h"

#include "motor.h"
#include "gpio_init.h"
#include "accelerometer.h"

int main()
{
	GPIO_configure();
	PWM_configure();
	
	uint8_t test = 0;
	
	Accelerometer_configure();
	
	while(1){
		if (test == 0)
		{
			TIM1 -> CCR4 = 1500;
			test = 1;
		}
		else if (test == 1)
		{
			TIM1 -> CCR4 = 1100;
			test = 2;
	}
	else if (test == 2){
		
		TIM1 -> CCR4 = 1500+(2*400);
		test = 0;
	}
}
}







