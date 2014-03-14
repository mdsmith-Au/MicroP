/**
 * @file temp_processing.c
 * Implementation of the temperature processor.
 * Simply encapsulates the functionality of other procedures 
 * to measure, filter, and convert the temperature into a 
 * usable format.
 */

#include "temp_processing.h"

/* 1. Get raw temperature data
 * 2. Filter the data
 * 3. Convert the filtered data to degrees Celsius 
 */
float getAndAverageTemp(FilterStruct *filter) {
	return convertToC(filterSMA(getTemp(), filter));
}
