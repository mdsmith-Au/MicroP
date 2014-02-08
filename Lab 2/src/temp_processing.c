#include "temp_processing.h"

float getAndAverageTemp(void) {
	
	/* Get raw temperature data -> average out in filter -> convert to deg. C -> return */
	return convertToC(processTemp(getTemp()));
}
