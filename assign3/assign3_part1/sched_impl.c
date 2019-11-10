#include "scheduler.h"
#include "sched_impl.h"
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <assert.h>
sem_t controlSem; //semaphore to control how many threads are in the queue at a time
sem_t cpuSem; //To allow 1 thread at a time to use the CPU (acts as mutex);
sem_t emptySem; //acts exactly opposite to controlSem. makes sure queue is not empty. 
/* Fill in your scheduler implementation code below: */
//Thread Stuff
/* Initialize a thread_info_t */
static void init_thread_info(thread_info_t *info, sched_queue_t *queue)
{
	printf("**INITIALIZING THREAD\n");
	info->queue = queue->list;
	info->queueData = NULL;
	/*...Code goes here...*/
}

/* Release the resources associated with a thread_info_t */
static void destroy_thread_info(thread_info_t *info)
{
	printf("**DESTROYING THREAD\n");
	free(info->queueData);
	/*...Code goes here...*/
}

/* Block until the thread can enter the scheduler queue. */
static void enter_sched_queue(thread_info_t *info)
{
	printf("**ENTERING QUEUE\n");
	sem_wait(&controlSem);
	info->queueData = (list_elem_t*)malloc(sizeof(list_elem_t));
	list_elem_init(info->queueData, (void*)info);
	list_insert_tail(info->queue, info->queueData);
	sem_post(&emptySem);
	sem_init(&info->runWorker,0,0);
	sem_wait(&info->runWorker); //sleep thread in queue until it has been scheduled to begin.
}

/* Remove the thread from the scheduler queue. */
static void leave_sched_queue(thread_info_t *info)
{
	printf("**LEAVING QUEUE\n");
	list_remove_elem(info->queue, info->queueData);
	sem_post(&controlSem);
}

/* While on the scheduler queue, block until thread is scheduled. */
static void wait_for_cpu(thread_info_t *info)
{
	printf("**WAITING TO BE SCHEDULED\n");
	sem_wait(&info->runWorker);
	printf("**SCHEDULED SUCCESSFULLY\n");
}
/* Voluntarily relinquish the CPU when this thread's timeslice is
 * over (cooperative multithreading). */
static void release_cpu(thread_info_t *info)
{
	printf("**RELEASING CPU\n");
	sem_post(&cpuSem);
	sched_yield();
}

//Queue Stuff
/* Initialize a sched_queue_t */
static void init_sched_queue(sched_queue_t *queue, int queue_size)
{
	printf("**INITIALIZING QUEUE\n");
	if (queue_size <= 0) {
		printf("Queue must have a size > 0\n");
		exit(-1); //exit entire program if queue has a size of zero
	}
	queue->currentWorker = NULL;
	queue->nextWorker = NULL;
	queue->list = (list_t*) malloc(sizeof(list_t));
	list_init(queue->list);
	sem_init(&controlSem, 0, queue_size);
	sem_init(&cpuSem,0,1);
	sem_init(&emptySem,0,0);
	/*...Code goes here...*/
}

/* Release the resources associated with a sched_queue_t */
static void destroy_sched_queue(sched_queue_t *queue)
{
	printf("**DESTROYING QUEUE\n");
	list_elem_t * temp;
	while ((temp = list_get_head(queue->list)) != NULL) {
		list_remove_elem(queue->list, temp);
	}//May not work as thread_info is deleted before this
	free(queue->list);
	/*...Code goes here...*/
}

/* Allow a worker thread to execute. */
static void wake_up_worker(thread_info_t *info)
{
	printf("**WAKING UP WORKER\n");
	sem_post(&info->runWorker);
}

/* Block until the current worker thread relinquishes the CPU. */
static void wait_for_worker(sched_queue_t *queue)
{
	printf("**WAITING FOR CPU\n");
	sem_wait(&cpuSem);//Maybe
	printf("**CPU FREE\n");
}

/* Select the next worker thread to execute.
 * Returns NULL if the scheduler queue is empty. */
static thread_info_t * next_worker(sched_queue_t *queue)
{
	printf("**SELECTING NEXT WORKER\n");
	if(list_size(queue->list) == 0)
		return NULL;

	if(queue->nextWorker == NULL) {
		queue->currentWorker = list_get_head(queue->list);
	} else {
		queue->currentWorker = queue->nextWorker;
	}

	queue->nextWorker = queue->currentWorker->next;
	printf("**NEXT WORKER SELECTED\n");
	return (thread_info_t*) queue->currentWorker->datum;
}

/* Block until at least one worker thread is in the scheduler queue. */
static void wait_for_queue(sched_queue_t *queue)
{
	printf("**WAITING FOR QUEUE TO FILL UP\n");
	sem_wait(&emptySem);
	printf("**QUEUE IS NOW FILLED\n");
}
/*...More functions go here...*/

/* You need to statically initialize these structures: */
sched_impl_t sched_fifo = {
	{ init_thread_info, destroy_thread_info, enter_sched_queue, leave_sched_queue, wait_for_cpu, release_cpu   /*, ...etc... */ }, 
	{ init_sched_queue, destroy_sched_queue, wake_up_worker, wait_for_worker, next_worker, wait_for_queue /*, ...etc... */ } },
sched_rr = {
	{ init_thread_info, destroy_thread_info, enter_sched_queue, leave_sched_queue, wait_for_cpu, release_cpu  /*, ...etc... */ }, 
	{ init_sched_queue, destroy_sched_queue, wake_up_worker, wait_for_worker, next_worker, wait_for_queue /*, ...etc... */ } };
