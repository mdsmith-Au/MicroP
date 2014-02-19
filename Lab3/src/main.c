#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "math.h"
#include "atan_LUT.h"

//NOTE TO STUDENTS: Uncomment the version of drivers related to your sensor
//#include "lis3dsh.h"
#include "stm32f4_discovery_lis302dl.h"

#include "motor.h"
#include "gpio_init.h"

int main()
{
	GPIO_configure();
	PWM_configure();
	
	uint8_t test = 0;
	
	while(1){
		if (test == 0)
		{
			TIM1 -> CCR4 = 2000;
			test = 1;
		}
		else if (test == 1)
		{
			TIM1 -> CCR4 = 17000;
			test = 2;
	}
	else if (test == 2){
		
		TIM1 -> CCR4 = 1;
		test = 0;
	}
}
}







