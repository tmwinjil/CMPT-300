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
	info->queue = queue->list;
	info->queueData = NULL;
}

/* Release the resources associated with a thread_info_t */
static void destroy_thread_info(thread_info_t *info)
{
	free(info->queueData);
}

/* Block until the thread can enter the scheduler queue. */
static void enter_sched_queue(thread_info_t *info)
{
	sem_wait(&controlSem);
	info->queueData = (list_elem_t*)malloc(sizeof(list_elem_t));
	list_elem_init(info->queueData, (void*)info);
	list_insert_tail(info->queue, info->queueData);
	if(list_size(info->queue) == 1)//list was previously empty and now is not
		sem_post(&emptySem);
	sem_init(&info->runWorker,0,0);
}

/* Remove the thread from the scheduler queue. */
static void leave_sched_queue(thread_info_t *info)
{
	list_remove_elem(info->queue, info->queueData);
	sem_post(&controlSem);
}

/* While on the scheduler queue, block until thread is scheduled. */
static void wait_for_cpu(thread_info_t *info)
{
	/*int a;
	sem_getvalue(&info->runWorker,&a);
	printf("**a = %d\n",a);*/
	sem_wait(&info->runWorker);
}
/* Voluntarily relinquish the CPU when this thread's timeslice is
 * over (cooperative multithreading). */
static void release_cpu(thread_info_t *info)
{
	sem_post(&cpuSem);
	sched_yield();
}

//Queue Stuff
/* Initialize a sched_queue_t */
static void init_sched_queue(sched_queue_t *queue, int queue_size)
{
	if (queue_size <= 0) {
		exit(-1); //exit entire program if queue has a size of zero
	}
	queue->currentWorker = NULL;
	queue->nextWorker = NULL;
	queue->list = (list_t*) malloc(sizeof(list_t));
	list_init(queue->list);
	sem_init(&controlSem, 0, queue_size);
	sem_init(&cpuSem,0,0);
	sem_init(&emptySem,0,0);
	/*...Code goes here...*/
}

/* Release the resources associated with a sched_queue_t */
static void destroy_sched_queue(sched_queue_t *queue)
{
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
	sem_post(&info->runWorker);
}

/* Block until the current worker thread relinquishes the CPU. */
static void wait_for_worker(sched_queue_t *queue)
{
	sem_wait(&cpuSem);
}

/* Select the next worker thread to execute.
 * Returns NULL if the scheduler queue is empty. */

static thread_info_t * next_worker_rr(sched_queue_t *queue)
{
	//printf("**SELECTING NEXT\n");
	if(list_size(queue->list) == 0) {
		return NULL;
	}
	if(queue->currentWorker == NULL) {//queue was just empty and now has an item in it
		queue->currentWorker = list_get_head(queue->list);
	} else if (queue->nextWorker == NULL) {//the last currentWorker was the tail of the queue
		if (queue->currentWorker == list_get_tail(queue->list)) {//the previous working thread is still in the queue and is the tail
			queue->currentWorker = list_get_head(queue->list);
		} else {
			queue->currentWorker = list_get_tail(queue->list); //collect the new tail
		}
	} else {//next worker is a member of the list
		queue->currentWorker = queue->nextWorker;
	}
	queue->nextWorker = queue->currentWorker->next;
	return (thread_info_t*) queue->currentWorker->datum;
}

static thread_info_t * next_worker_fifo(sched_queue_t *queue) {
	if(list_size(queue->list) == 0) {
		return NULL;
	}
	else {
		return (thread_info_t*) (list_get_head(queue->list))->datum;
	}
}

/* Block until at least one worker thread is in the scheduler queue. */
static void wait_for_queue(sched_queue_t *queue)
{
	sem_wait(&emptySem);
}
/*...More functions go here...*/

/* You need to statically initialize these structures: */
sched_impl_t sched_fifo = {
	{ init_thread_info, destroy_thread_info, enter_sched_queue, leave_sched_queue, wait_for_cpu, release_cpu}, 
	{ init_sched_queue, destroy_sched_queue, wake_up_worker, wait_for_worker, next_worker_fifo, wait_for_queue} },
sched_rr = {
	{ init_thread_info, destroy_thread_info, enter_sched_queue, leave_sched_queue, wait_for_cpu, release_cpu}, 
	{ init_sched_queue, destroy_sched_queue, wake_up_worker, wait_for_worker, next_worker_rr, wait_for_queue} };
