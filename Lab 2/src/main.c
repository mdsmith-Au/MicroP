#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include "gpio_example.h"
#include "adc_init.h"

int main()
{

	GPIO_example_config();


	GPIO_SetBits(GPIOD, GPIO_Pin_12);

	/* Configure ADC
	 * Temp sensor @ ADC1_IN16*/
	ADC_Config();
	
	while(1) {
		ADC_SoftwareStartConv(ADC1); //Starting Conversion
		
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); // Wait for ADC to finish by setting flag
		
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC); //Clear the above flag
		uint16_t temp = ADC_GetConversionValue(ADC1); // Result available in ADC1->DR
		printf("Temp: %u %x\n", temp, temp);
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


