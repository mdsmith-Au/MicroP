/**
 * @file filter.c
 * Filter implementation using simple moving averages.
 */
 
#include "filter.h"

/* This uses a simple moving average algorithm to smooth out 
 * any outliers in the temperature data.
 */
int filterSMA(int data, FilterStruct *filterBuffer) {
    /* Data at next position to be used */
	const int lastData = filterBuffer->buffer[filterBuffer->position]; 
	
	/* If the next position already has a non-zero value 
       (aka the filter depth has already been reached), move the 
       window along by removing the oldest element from the sum. */
	if (lastData != 0) {
		filterBuffer->sum -= lastData;
	}
	
    /* Store the new data and increment the position */
	filterBuffer->buffer[filterBuffer->position] = data; 
	filterBuffer->position = (filterBuffer->position + 1) % FILTER_DEPTH;
	
	/* Tracks the number of data samples currently in the filter.
       Maxes at FILTER_DEPTH */
	if (filterBuffer->num_samples < FILTER_DEPTH) {
		filterBuffer->num_samples++;
	}
	
    /* Add new data to sum */
	filterBuffer->sum += data; 
	
    /* Compute the average and return */
	return filterBuffer->sum / filterBuffer->num_samples;
}

/* Set everything to 0 */
void initFilterBuffer(FilterStruct *filterBuffer) {
	memset(filterBuffer, 0, sizeof(*filterBuffer));
}
