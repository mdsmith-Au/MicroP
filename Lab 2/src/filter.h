/**
 * @file filter.h
 * Header file for the filter.
 */

/* @TODO #ifndef */
#include <string.h> /* For memset */
#include <stdint.h>

/**
 * Controls how many elements to process. Chosen arbitrarily.
 * @TODO: better explanation on what this does, significance, ...
 */
#define FILTER_DEPTH 32

uint16_t processTemp(uint16_t temp);

/**
 * Zeros out the filter buffer.
 */ 
void initializeBuffer(void);
