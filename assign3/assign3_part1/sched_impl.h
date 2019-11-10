#ifndef	__SCHED_IMPL__H__
#define	__SCHED_IMPL__H__
#include <semaphore.h>
#include "list.h"

struct thread_info {
	sched_queue_t* queue;
	list_elem_t* queueData;
	/*...Fill this in...*/
};

struct sched_queue {
	int maxSize;
	int size;
	
	//list_elem_t* pos;//used to show the current thread being executed
	list_t* list
	/*...Fill this in...*/
};

#endif /* __SCHED_IMPL__H__ */