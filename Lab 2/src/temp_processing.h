/**
 * @file temp_processing.h
 * Header file the temperature processor.
 */
 
#include "get_temp.h"
#include "filter.h"

/**
 * Makes calls to other functions to appropriate capture, 
 * filter, and convert temperature data.
 */
float getAndAverageTemp(void);
