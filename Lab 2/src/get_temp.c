#include "get_temp.h"
#include <stdio.h>

uint8_t calibrated_GLB = 0;
float slope_GLB = 0;

float getTemp(void) {
	
		if (!calibrated_GLB) {
			printf("Temperature not calibrated! Using average value.\n");
			slope_GLB = AVG_SLOPE;
		}
		
		ADC_SoftwareStartConv(ADC1); //Starting Conversion
		
		GPIO_SetBits(GPIOD, GPIO_Pin_13); // Light orange LED while waiting
			
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); // Wait for ADC to finish by setting flag
			
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC); //Clear the above flag
		GPIO_ResetBits(GPIOD, GPIO_Pin_13); // Reset orange LED
			
		/* Convert to deg. C using equation found 
		 * in ARM Doc ID 018909 Rev 1 and data from Table 69 in
		 * and Doc ID 022152 Rev 4*/
		
		uint16_t temp = ADC_GetConversionValue(ADC1); // Get temp from ADC.  12 bit, aligned right -> can read directly
		
		float tempInMilliVolts  = (temp/VREF_INT) * VREF; // Convert to an actual voltage (i.e. 0.8V)
		tempInMilliVolts = ((tempInMilliVolts - V_25)/slope_GLB) + 25; //D o conversion per formula mentioned above
		
		return tempInMilliVolts;
	
}

/* Temperature calibration based on factory data stored in predefined mem locations */
void calibrateTemp(void) {
	uint16_t const * const tempLow = (uint16_t*)0x1FFF7A2C;
	uint16_t const * const tempHigh = (uint16_t*)0x1FFF7A2E;
	
	/* Two calibration points @ 3.3V; 30C and 110C.  Use data to calculate slope.
	 * Find difference, then divide by and multiply by voltage in ADC and normal format respectively.
	 * Then, simply divide by the temperature difference.  Basic high school math, except for the conversion.*/
	slope_GLB = (((*tempHigh - *tempLow)/VREF_INT) * VREF_FACTORY)/TEMP_DIFF_FACTORY;
	calibrated_GLB = 1;
}
