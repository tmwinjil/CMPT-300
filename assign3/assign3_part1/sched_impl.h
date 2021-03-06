#ifndef	__SCHED_IMPL__H__
#define	__SCHED_IMPL__H__
#include <semaphore.h>
#include "list.h"

struct thread_info {
	list_t* queue;
	list_elem_t* queueData;
	sem_t runWorker;//Semaphore to activate or deactivate worker thread.
};

struct sched_queue {
	list_elem_t* currentWorker;
	//Insert all semaphores for queue here
	list_t* list;
};

#endif /* __SCHED_IMPL__H__ */