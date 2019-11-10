#include "scheduler.h"
#include "sched_impl.h"
#include <stdio.h>

sem_t controlSem; //semaphore to control how many threads are in the queue at a time
/* Fill in your scheduler implementation code below: */
//Thread Stuff
/* Initialize a thread_info_t */
static void init_thread_info(thread_info_t *info, sched_queue_t *queue)
{
	info->queue = queue;
	info->queueData = NULL;
	/*...Code goes here...*/
}

/* Release the resources associated with a thread_info_t */
static void destroy_thread_info(thread_info_t *info)
{
	free(info->queueData);
	/*...Code goes here...*/
}

/* Block until the thread can enter the scheduler queue. */
static void enter_sched_queue(thread_info_t *info)
{
	sem_wait(&controlSem);
	info->queueData = (list_elem_t*) malloc(sizeof(list_elem_t*));
	list_elem_init(info->queueData, (void*)info);
	list_insert_tail(info->queue, info->queueData);
	info->queue->size++;
}

/* Remove the thread from the scheduler queue. */
static void leave_sched_queue(thread_info_t *info)
{
	list_remove_elem(info->queue, info->queueData);
	info->queue->size--;
	sem_post(&controlSem);
}

/* While on the scheduler queue, block until thread is scheduled. */
static void wait_for_cpu(thread_info_t *info)
{

}
/* Voluntarily relinquish the CPU when this thread's timeslice is
 * over (cooperative multithreading). */
static void release_cpu(thread_info_t *info)
{
	sched_yield();
}

//Queue Stuff
/* Initialize a sched_queue_t */
static void init_sched_queue(sched_queue_t *queue, int queue_size)
{
	if (queue_size <= 0) {
		printf("Queue must have a size > 0\n");
		exit(-1); //exit entire program if queue has a size of zero
	}
	queue->list = (list_t*) malloc(sizeof(list_t));
	list_init(queue->list);
	queue->maxSize = queue_size;
	//queue->pos = NULL;
	sem_init(&controlSem, 0, queue_size);
	/*...Code goes here...*/
}

/* Release the resources associated with a sched_queue_t */
static void destroy_sched_queue(sched_queue_t *queue)
{
	list_elem_t * temp;
	while ((temp = list_get_head(queue->list)) != NULL) {
		list_remove_elem(queue, temp);
	}//May not work as thread_info is deleted before this
	free(queue->list);
	/*...Code goes here...*/
}

/* Allow a worker thread to execute. */
static void wake_up_worker(thread_info_t *info)
{


}

/* Block until the current worker thread relinquishes the CPU. */
static void wait_for_worker(sched_queue_t *queue)
{
	sem_wait(&(queue->cpuSem));
}

/* Select the next worker thread to execute.
 * Returns NULL if the scheduler queue is empty. */
static thread_info_t * next_worker(sched_queue_t *queue)
{
	if(queue->size == 0)
		return NULL;
	return list_get_head(queue->list);//head always runs next
}

/* Block until at least one worker thread is in the scheduler queue. */
static void wait_for_queue(sched_queue_t *queue)
{

}
/*...More functions go here...*/

/* You need to statically initialize these structures: */
sched_impl_t sched_fifo = {
	{ init_thread_info, destroy_thread_info, enter_sched_queue, leave_sched_queue, wait_for_cpu, release_cpu   /*, ...etc... */ }, 
	{ init_sched_queue, destroy_sched_queue, wake_up_worker, wait_for_worker, next_worker, wait_for_queue /*, ...etc... */ } },
sched_rr = {
	{ init_thread_info, destroy_thread_info, enter_sched_queue, leave_sched_queue, wait_for_cpu, release_cpu  /*, ...etc... */ }, 
	{ init_sched_queue, destroy_sched_queue, wake_up_worker, wait_for_worker, next_worker, wait_for_queue /*, ...etc... */ } };
