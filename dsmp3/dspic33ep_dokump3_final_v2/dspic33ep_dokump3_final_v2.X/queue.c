#include <xc.h>
#include "queue.h"

volatile queue_post_t queue_head_left;
volatile queue_post_t queue_tail_left;

volatile queue_post_t queue_head_right;
volatile queue_post_t queue_tail_right;

#ifdef USE_EDS
__eds__ volatile queue_val_t __attribute__((eds, section("pcmQueueLeft"), space(ymemory))) queue_members_left[QUEUE_CAPACITY];
__eds__ volatile queue_val_t __attribute__((eds, section("pcmQueueRight"), space(ymemory))) queue_members_right[QUEUE_CAPACITY];
#else
volatile queue_val_t queue_members_left[QUEUE_CAPACITY];
volatile queue_val_t queue_members_right[QUEUE_CAPACITY];
#endif

void queue_clear_left()
{
    queue_head_left = 0;
    queue_tail_left = 0;
}

char queue_put_left(queue_val_t data) {
    queue_post_t next_head = (queue_head_left + 1) % QUEUE_CAPACITY;
    if (next_head != queue_tail_left) {
	    // add item to queue	
        queue_members_left[queue_head_left] = data;
        queue_head_left = next_head;
        return 1;
    } else {	
        return 0;
    }
}

char queue_get_left(queue_val_t *data) {
    if (queue_head_left != queue_tail_left) {
	    // retrieve item from queue
        *data = queue_members_left[queue_tail_left];
        queue_tail_left = (queue_tail_left + 1) % QUEUE_CAPACITY;
        return 1;
    } else {	
    	// queue empty  
        return 0;
    }
}

queue_post_t queue_get_len_left()
{
	queue_post_t q_head = queue_head_left;
	queue_post_t q_tail = queue_tail_left;
			
	if (q_head > q_tail)
		return q_head - q_tail;
		
	if (q_tail > q_head)
		return q_head + QUEUE_CAPACITY - q_tail;
		
	return 0;			
}

void queue_clear_right()
{
    queue_head_right = 0;
    queue_tail_right = 0;
}

char queue_put_right(queue_val_t data) {
    queue_post_t next_head = (queue_head_right + 1) % QUEUE_CAPACITY;
    if (next_head != queue_tail_right) {
	    // add item to queue	
        queue_members_right[queue_head_right] = data;
        queue_head_right = next_head;
        return 1;
    } else {	
        // LATAbits.LATA0 = !LATAbits.LATA0;
        
        return 0;
    }
}

char queue_get_right(queue_val_t *data) {
    if (queue_head_right != queue_tail_right) {
	    // retrieve item from queue
        *data = queue_members_right[queue_tail_right];
        queue_tail_right = (queue_tail_right + 1) % QUEUE_CAPACITY;
        return 1;
    } else {	
        // LATAbits.LATA0 = !LATAbits.LATA0;
        
    	// queue empty  
        return 0;
    }
}

queue_post_t queue_get_len_right()
{
	queue_post_t q_head = queue_head_right;
	queue_post_t q_tail = queue_tail_right;
			
	if (q_head > q_tail)
		return q_head - q_tail;
		
	if (q_tail > q_head)
		return q_head + QUEUE_CAPACITY - q_tail;
		
	return 0;			
}