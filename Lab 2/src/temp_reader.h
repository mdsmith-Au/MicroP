/**
 * @file temp_reader.h
 * Header file for the temperature reader.
 */
 
#include "stm32f4xx_adc.h"

#define V_25         (float)760     /* From documentation, Doc ID 022152 Rev 4 */
#define AVG_SLOPE           2.5     /* From documentation, Doc ID 022152 Rev 4 */
#define VREF_INT   (float)0xFFF     /* 4095 a.k.a. VDD value as raw integer*/
#define VREF               3012     /* 3.012V VDD on USB, measured */
#define VREF_FACTORY       3300     /* 3.3V from Doc ID 022152 Rev 4 */
#define TEMP_DIFF_FACTORY    80     /* 80C between points, from above document */

/**
 * Obtains the temperature as measured by the temperature sensor.
 * @return a relative temperature voltage value (mV)
 */
uint16_t getTemp(void);

/**
 * Calibrates the temperature sensor such that low and high relative temperature values
 * are configured. These low and high temperatures will serve as reference points when 
 * converting temperatures from millivolts to degrees Celsius.
 */
void calibrateTempSensor(void);

/**
 * Converts a voltage, which represents a relative temperature value, into 
 * a more readable temperature value (in degrees Celsius).
 * @param temp the temperature voltage as reported by the temperature sensor
 * @return the temperature in degrees Celsius
 */
float convertToC(uint16_t temp);
