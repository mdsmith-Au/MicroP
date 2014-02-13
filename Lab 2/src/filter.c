/**
 * @file filter.c
 * Filter implementation using simple moving averages.
 */
 
#include "filter.h"

/** Manages the filter internal state. */
FilterStruct filterBuffer;

/* Temperature filter implementation.
 * This uses a simple moving average algorithm to smooth out 
 * any outliers in the temperature data.
 */
uint16_t filterTemperature(uint16_t temp) {
    /* Temp at next position to be used */
	const uint16_t lastTemp = filterBuffer.buffer[filterBuffer.position]; 
	
	/* If the next position already has a non-zero value 
       (aka the filter depth has already been reached), move the 
       window along by removing the oldest element from the sum. */
	if (lastTemp != 0) {
		filterBuffer.sum -= lastTemp;
	}
	
    /* Store the new temperature and increment the position */
	filterBuffer.buffer[filterBuffer.position] = temp; 
	filterBuffer.position = (filterBuffer.position + 1) % FILTER_DEPTH;
	
	/* Tracks the number of data samples currently in the filter.
       Maxes at FILTER_DEPTH */
	if (filterBuffer.num_samples < FILTER_DEPTH) {
		filterBuffer.num_samples++;
	}
	
    /* Add new temperature to sum */
	filterBuffer.sum += temp; 
	
    /* Compute the average and return */
	return filterBuffer.sum / filterBuffer.num_samples;
}

/* Set everything to 0 */
void initFilterBuffer() {
	memset(&filterBuffer, 0, sizeof(filterBuffer));
}
