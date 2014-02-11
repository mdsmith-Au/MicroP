#include "stm32f4xx_adc.h"

#define V_25         (float)760     /* From documentation, Doc ID 022152 Rev 4 */
#define AVG_SLOPE           2.5     /* From documentation, Doc ID 022152 Rev 4 */
#define VREF_INT   (float)0xFFF     /* 4095 a.k.a. VDD value as raw integer*/
#define VREF               3012     /* 3.012V VDD on USB, measured */

#define VREF_FACTORY       3300     /* 3.3V from Doc ID 022152 Rev 4 */
#define TEMP_DIFF_FACTORY    80     /* 80C between points, from above document */

uint16_t getTemp(void);
void calibrateTemp(void);
float convertToC(uint16_t temp);
