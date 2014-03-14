/**
 * @file temp_reader.c
 * Temperature reader implementation.
 */

#include "temp_reader.h"

/** Flag to denote if the temperature sensor has been calibrated. */
uint8_t isCalibrated = 0;

/** Stores the temperature vs voltage slope. */
float tempVsVoltageSlope = 0;

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
 *   tempInDegC = ((tempInMilliVolts - V_25)/tempVsVoltageSlope) + 25
 * where:
 *   tempInMilliVolts = (temp/VREF_INT) * VREF
 *   V_25 is the reference voltage at 25 deg C (given)
 *   tempVsVoltageSlope is the slope between temperature and voltage reading,  
 *        computed during temp sensor calibration, or is simply the average slope
 *
 * Reference:
 * - Documents are Doc ID 018909 Rev 1 with data from
 * - Table 68 in Doc ID 022152 Rev 4 
 */
float convertToC(uint16_t temp) { 
	// If GLB is not calibrated, use average value for slope
	if (!isCalibrated) {
        tempVsVoltageSlope = AVG_SLOPE; 
    }
    
	return ((temp/VREF_INT) * VREF - V_25)/tempVsVoltageSlope + 25;
}

/* Temperature sensor calibration based on factory data stored in predefined mem locations */
void calibrateTempSensor() {
	// Temperature sensor calibration values
	uint16_t const * const tempLow  = (uint16_t*)0x1FFF7A2C;    // Mem address where factory TS_CAL1 is stored
	uint16_t const * const tempHigh = (uint16_t*)0x1FFF7A2E;    // Mem address where factory TS_CAL2 is stored
	
	/* Two calibration points @ 3.3V; 30C and 110C.  Use data to calculate slope.
	 * Find difference, then divide by and multiply by voltage in ADC and normal format respectively.
	 * Then, simply divide by the temperature difference.*/
	tempVsVoltageSlope = (((*tempHigh - *tempLow)/VREF_INT) * VREF_FACTORY)/TEMP_DIFF_FACTORY;
    
    // Set to true so that we use this value instead of the default average when converting
	isCalibrated = 1; 
}
