/* Authors: 	Nicholas Destounis
 *						Nikolaos Bukas
 *
 * Lab 2
 * Date:			February 13, 2014
 *
 * Course: 		ECSE 426 - Microprocessor Systems
 * Professor: Mark Coates
*/

 
/*
 * Inspired by implementation at http://en.wikipedia.org/wiki/Circular_queue
 */

#include "circular_queue.h"

void initialize_queue(Queue *q, int size)
{
    q->size  = size + 1;
    q->first = 0;
    q->last   = 0;
		q->numElements = 0;
	
		int i;
	
		//Initialize arrays to 0;
		for (i = 0; i < q->size; i++)
		{
			q->elements[i] = 0;
		}
}
 
void enqueue(Queue *q, int *element) 
{
    q->elements[q->last] = *element;
    q->last = (q->last + 1) % q->size;
	
		//case queue is full, overwrite
    if (q->last == q->first)
		{
        q->first = (q->first + 1) % q->size; 
		}
		else
		{
				q->numElements++;
		}
}
 
void dequeue(Queue *q, int *element) 
{
    *element = q->elements[q->first];
    q->first = (q->first + 1) % q->size;
	
		q->numElements--;
}

int is_full(Queue *q) 
{
    return (q->last + 1) % q->size == q->first;
}
 
int is_empty(Queue *q) 
{
    return q->last == q->first;
}
