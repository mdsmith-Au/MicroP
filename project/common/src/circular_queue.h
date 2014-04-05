/*
 * Inspired by implementation at http://en.wikipedia.org/wiki/Circular_queue
*/

/*!
 @file circular_queue.h
 @author Nicholas Destounis
 @author Nikolaos Bukas
 @author Michael Smith
 @author Kevin Dam
 @brief This is an implementation of an array based circular buffer.
 */
 
/*! @addtogroup Microp Project Group 1
 *  @{
 */

#define BUFFER_SIZE 16 /*!< Size of the underlying array */

/**
* A structure to represent the circular queue
*/
typedef struct {
    int size; 	/**< the size */
    int first; 	/**< the index of the head */
    int last;   /**< the index of the tail */
	int numElements; /**< the current number of elements in the queue */
	int elements[BUFFER_SIZE + 1]; /**< the array containing queue elements */
} Queue;

/*!
 Initialize the queue. 
 @param[in,out] q A pointer to the queue struct
 @param[in] size The maximum number of elements in the queue
 */
void initialize_queue(Queue *q, int size);

/*!
 Enqueues an element to the tail of the queue. If the queue is full, the head element is discarded. 
 @param[in,out] q A pointer to the queue struct
 @param[in] element A pointer to the element to enqueue
 */
void enqueue(Queue *q, int *element);

/*!
 Dequeues an element from the head of the queue.
 @param[in,out] q A pointer to the queue struct
 @param[out] element The location to store the dequeued value
 */
void dequeue(Queue *q, int *element);

/*!
 Determines whether the queue is full or not. Returns 1 if full and 0 otherwise
 @param[in] q A pointer to the queue struct
 */
int is_full(Queue *q);

/*!
 Determines whether the queue is empty or not. Returns 1 if empty and 0 otherwise
 @param[in] q A pointer to the queue struct
 */
int is_empty(Queue *q);

//! @} 
