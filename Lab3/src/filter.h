#ifndef _TEMP_FILTER_H_
#define _TEMP_FILTER_H_

/**
 * @file filter.h
 * Header file for the filter.
 * The filter is used to smooth out the noisy components that may be 
 * introduced while reading sensor data..
 */

/* @TODO #ifndef */
#include <string.h> /* For memset */
#include <stdint.h> /* For alternate type definitions */

/**
 * Controls how many elements to process (the window size of the filter).
 * This value was determined through experimentation and analysis of different 
 * filter depths. A larger filter depth generated smoother results, but with a 
 * tradeoff of time. A smaller filter depth was fast but not as smooth. The 
 * current filter depth value was chosen to balance the smoothness and speed factors.
 */
#define FILTER_DEPTH 18

/**
 * Keeps track of the internal state of the filter buffer.
 */
typedef struct {
  uint8_t position;                   ///< Index pointing to next position to be filled
	uint8_t num_samples;                ///< Number of samples in buffer.  Used when filling for the first time up to FILTER_DEPTH
	int buffer[FILTER_DEPTH];      ///< Buffer storing old (and new) temperature values for averaging
	int sum;                            ///< Sum of all values in buffer
} FilterStruct;

/**
 * Sensor data filtering algorithm.
 * @param data New data point to be filterd
 * @return the filtered data point (averaged)
 */
int filterSMA(int data, FilterStruct *filterBuffer);

/**
 * Fills the filter buffer with zeroes.
 */ 
void initFilterBuffer(FilterStruct *filterBuffer);

#endif
