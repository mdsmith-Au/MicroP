/**
 * @file temp_reader.c
 * Temperature reader implementation.
 */

#include "temp_reader.h"

/** Flag to denote if the GLB has been calibrated. */
uint8_t calibrated_GLB = 0;

/** Slope of the GLB. */
float slope_GLB = 0;

uint16_t getTemp() {
    // Starting Conversion
    ADC_SoftwareStartConv(ADC1); 
    
    // Wait for ADC to finish by setting flag
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); 
    
    // Clear the above flag
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC); 
    
    // Get temp from ADC. 12 bit, aligned right -> can read directly
    return ADC_GetConversionValue(ADC1); 
}

/**
 * Formula mentioned in ARM documents with voltage:
 *   tempInDegC = ((tempInMilliVolts - V_25)/slope_GLB) + 25
 * where:
 *   tempInMilliVolts = (temp/VREF_INT) * VREF
 *   V_25 is the reference voltage at 25 deg C (given)
 *   slope_GLB is the slope of the GLB, computed during temp sensor 
 *             calibration, or is simply the average slope
 *
 * Reference:
 * - Documents are Doc ID 018909 Rev 1 with data from
 * - Table 68 in Doc ID 022152 Rev 4 
 */
float convertToC(uint16_t temp) {
    // If GLB is ot calibrated, use average value for slope
	if (!calibrated_GLB) {
        slope_GLB = AVG_SLOPE; 
    }
    
	return ((temp/VREF_INT) * VREF - V_25)/slope_GLB + 25;
}

/* Temperature sensor calibration based on factory data stored in predefined mem locations */
void calibrateTempSensor() {
	uint16_t const * const tempLow  = (uint16_t*)0x1FFF7A2C;
	uint16_t const * const tempHigh = (uint16_t*)0x1FFF7A2E;
	
	/* Two calibration points @ 3.3V; 30C and 110C.  Use data to calculate slope.
	 * Find difference, then divide by and multiply by voltage in ADC and normal format respectively.
	 * Then, simply divide by the temperature difference.*/
	slope_GLB = (((*tempHigh - *tempLow)/VREF_INT) * VREF_FACTORY)/TEMP_DIFF_FACTORY;
    
    // Set to true so that we use this value instead of the default average when converting
	calibrated_GLB = 1; 
}
