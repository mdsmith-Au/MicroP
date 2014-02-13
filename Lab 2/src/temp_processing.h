/**
 * @file temp_processing.h
 * Header file the temperature processor.
 */
 
#include "temp_reader.h"
#include "filter.h"

/**
 * Makes calls to other functions to appropriate capture, 
 * filter, and convert temperature data.
 */
float getAndAverageTemp(void);
