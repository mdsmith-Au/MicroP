#include "filter.h"

// @TODO: May want to move this to its own file, or at least the header.
// @TODO: Also, need to format for correct coding convention.
typedef struct
{
  uint8_t position; //Index pointing to next position to be filled
	uint8_t num_samples; //Number of samples in buffer.  Used when filling for the first time up to FILTER_DEPTH
	uint16_t buffer[FILTER_DEPTH]; //Buffer storing old (and new) temperature values for averaging
	int sum; //Sum of all values in buffer
	
}filterStruct;

filterStruct filterBuffer;

uint16_t processTemp(uint16_t temp) {

	const uint16_t lastTemp = filterBuffer.buffer[filterBuffer.position]; // Temp at next position to be used.
	
	/* Position already filled; remove it from sum */
	if (lastTemp != 0) {
		filterBuffer.sum -= lastTemp;
	}
	
	filterBuffer.buffer[filterBuffer.position] = temp; // Store new value in buffer

	filterBuffer.position = (filterBuffer.position + 1) % FILTER_DEPTH; // Increment position index counter 
	
	/* Increment non-zero sample counter if not already at max */
	if (filterBuffer.num_samples < FILTER_DEPTH) {
		filterBuffer.num_samples++;
	}
	
	filterBuffer.sum += temp; // Add new temperature to sum 
	
	return filterBuffer.sum/filterBuffer.num_samples; // Divide sum by num. of samples a.k.a. the average 
}

/* Set everything to 0 */
void initializeBuffer(void) {
	memset(&filterBuffer, 0, sizeof(filterBuffer));
	
}
