/* Authors: 	Nicholas Destounis
 *						Nikolaos Bukas
 *
 * Lab 2
 * Date:			February 13, 2014
 *
 * Course: 		ECSE 426 - Microprocessor Systems
 * Professor: Mark Coates
*/

#include "filter.h"

void init_filter(Filter *f, Queue *q, int depth)
{
	f->depth = depth;
	f->avg = 0;
	f->sum = 0;
	f->buffer = q;
}

void add_measurement(Filter *f, int measurement)
{
	int currentNumElements = f->buffer->numElements;
	int droppedElement;
	
	f->sum += measurement;
	
	//If the buffer is full, a number will be dropped off. Remove from sum.
	if (currentNumElements == (f->buffer->size - 1))
	{
		dequeue(f->buffer, &droppedElement);
		f->sum -= droppedElement;
	}
	
	enqueue(f->buffer, &measurement);
	
	currentNumElements = f->buffer->numElements;
	
	f->avg = ((float)f->sum)/ f->depth; //This will be a shift with depths of powers of 2
}
