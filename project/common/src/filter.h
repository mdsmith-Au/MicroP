#include "circular_queue.h"

/*!
 @file filter.h
 @author Nicholas Destounis
 @author Nikolaos Bukas
 @brief This file contains an implementation of a moving average filter.
 */

/*! @addtogroup LAB2
 *  @{
 */
 
/**
* A structure to represent the filter
*/
typedef struct {
    Queue *buffer; /*! The underlying buffer holding past values */
		int depth; /*! The filter depth */
		int sum; /*! The total sum of elements in the filter */
		float avg; /*! The current average of the elements in the filter */
} Filter;

/*!
 Initialize the filter. 
 @param[in,out] f A pointer to the filter struct
 @param[in,out] q A pointer to the buffer struct
 @param[in] depth The filter depth
 */
void init_filter(Filter *f, Queue *q, int depth);

/*!
 Add a measurement to the filter
 @param[in,out] f A pointer to the filter struct
 @param[in] measurement The measurement to add
 */
void add_measurement(Filter *f, int measurement);

//! @}
