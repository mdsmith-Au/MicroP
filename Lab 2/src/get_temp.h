#include "stm32f4xx_adc.h"

#define V_25  (float)760
#define AVG_SLOPE  2.5
#define VREF_INT	(float)0xFFF /* 4095 a.k.a. 5V */
#define VREF	3100 /* 3.1V VDD, measured */

#define VREF_FACTORY 3300 /* 3.3V from Doc ID 022152 Rev 4 */
#define TEMP_DIFF_FACTORY 80 /* 80C between points */

uint16_t getTemp(void);
void calibrateTemp(void);
