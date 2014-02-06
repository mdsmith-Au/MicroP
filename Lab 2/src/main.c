#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include "gpio_example.h"
#include "adc_init.h"
#include "get_temp.h"

int main()
{

	GPIO_example_config();


	GPIO_SetBits(GPIOD, GPIO_Pin_12);

	/* Configure ADC
	 * Temp sensor @ ADC1_IN16*/
	ADC_Config();
	
	calibrateTemp();
	
	while(1) {
		printf("Temp: %u\n", getTemp());
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


