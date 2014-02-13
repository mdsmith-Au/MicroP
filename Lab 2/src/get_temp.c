#include "get_temp.h"

uint8_t calibrated_GLB = 0;
float slope_GLB = 0;

uint16_t getTemp(void) {
	
		ADC_SoftwareStartConv(ADC1); //Starting Conversion
		
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); // Wait for ADC to finish by setting flag
			
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC); //Clear the above flag
		
		return ADC_GetConversionValue(ADC1); // Get temp from ADC.  12 bit, aligned right -> can read directly
	
}

float convertToC(uint16_t temp) {
		
	if (!calibrated_GLB) {
			slope_GLB = AVG_SLOPE; // Not calibrated, use average value
		}
	
	/* Convert to voltage (i.e. 800mV):
	 * tempInMilliVolts = (temp/VREF_INT) * VREF
	 * Use formula mentioned in ARM documents with voltage:
	 * ((tempInMilliVolts - V_25)/slope_GLB) + 25
	 * Documents are Doc ID 018909 Rev 1 with data from
	 * Table 68 in Doc ID 022152 Rev 4 
	 */
	return ((((temp/VREF_INT) * VREF) - V_25)/slope_GLB) + 25;
	
}

/* Temperature sensor calibration based on factory data stored in predefined mem locations */
void calibrateTempSensor(void) {
	uint16_t const * const tempLow = (uint16_t*)0x1FFF7A2C;
	uint16_t const * const tempHigh = (uint16_t*)0x1FFF7A2E;
	
	/* Two calibration points @ 3.3V; 30C and 110C.  Use data to calculate slope.
	 * Find difference, then divide by and multiply by voltage in ADC and normal format respectively.
	 * Then, simply divide by the temperature difference.  Basic high school graph math, except for the conversion.*/
	slope_GLB = (((*tempHigh - *tempLow)/VREF_INT) * VREF_FACTORY)/TEMP_DIFF_FACTORY;
	calibrated_GLB = 1; //Set to true so that we use this value instead of the default average when converting
}
