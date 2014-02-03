#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include "gpio_example.h"


int main()
{

	GPIO_example_config();
	
	while(1) {

		GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
		GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
		printf("This is a TEST?");
		
	}
}


/*
	*	Configure ADC
	* Clock gating
	* Get temp via printf
	* max #  bits
	* data alignment: to the right
	* use independent mode
	* systick config
	* */


