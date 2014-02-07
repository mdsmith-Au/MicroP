#include "filter.h"


typedef struct
{
  uint8_t position;
	int buffer[FILTER_DEPTH];
	int sum;
	
}filterStruct;

filterStruct filterBuffer;

uint16_t processTemp(uint16_t temp) {
	
	filterBuffer.buffer[filterBuffer.position] = temp;
	filterBuffer.position++;
	filterBuffer.sum = filterBuffer.sum + temp;
	
	return filterBuffer.sum/FILTER_DEPTH;
}

/* Set everything to 0 */
void initializeBuffer(void) {
	memset(&filterBuffer, 0, sizeof(filterBuffer));
	
}
