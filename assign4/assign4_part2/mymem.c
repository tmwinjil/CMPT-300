#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>


/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

struct memoryList
{
  // doubly-linked list
  struct memoryList *last;
  struct memoryList *next;

  int size;            // How many bytes in this block?
  char alloc;          // 1 if this block is allocated,
                       // 0 if this block is free.
  void *ptr;           // location of block in memory pool.
};

strategies myStrategy = NotSet;    // Current strategy


size_t mySize;
size_t currentSize;
void *myMemory = NULL;

static struct memoryList *head;
static struct memoryList *next;

/* initmem must be called prior to mymalloc and myfree.

   initmem may be called more than once in a given exeuction;
   when this occurs, all memory you previously malloc'ed  *must* be freed,
   including any existing bookkeeping data.

   strategy must be one of the following:
		- "best" (best-fit)
		- "worst" (worst-fit)
		- "first" (first-fit)
		- "next" (next-fit)
   sz specifies the number of bytes that will be available, in total, for all mymalloc requests.
*/

void initmem(strategies strategy, size_t sz)
{
	myStrategy = strategy;
	/* all implementations will need an actual block of memory to use */
	mySize = sz;

	if (myMemory != NULL) free(myMemory); /* in case this is not the first time initmem2 is called */

	/* TODO: release any other memory you were using for bookkeeping when doing a re-initialization! */
	while(head != NULL) {
		struct memoryList* deadNode = head;
		head = head->next;
		free(deadNode);
	}

	myMemory = malloc(sz);
	
	/* TODO: Initialize memory management structure. */
	head = malloc(sizeof(struct memoryList));
	head->last = NULL;
	head->next = NULL;
	head->size = sz; 
	head->alloc = 0;
	head->ptr = myMemory;
	next = NULL;
	currentSize = 0;

}
/*************************************************************/

//Creates a block of free space after allocation.
void insertAfter(struct memoryList* ptr, size_t sz) {
	struct memoryList* newNode = malloc(sizeof(struct memoryList));
		newNode->next = ptr->next;
		newNode->last = ptr;
		newNode->alloc = 0;
		newNode->size = sz;
		newNode->ptr = ptr->ptr + ptr->size;
	if (sz == 0) {
		free(newNode);
		return;
	}
	if (ptr->next == NULL) {
		ptr->next = newNode;
	} else {
		if (ptr->next->alloc == 0) {
			ptr->next->size += sz;
			ptr->next->ptr -= sz;
			free(newNode);
		} else {
			ptr->next->last = newNode;
			ptr->next = newNode;
		}
	}
}

/*************************************************************/
/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */
void *mymalloc(size_t requested)
{
	assert((int)myStrategy > 0);
	struct memoryList *ptr, *temp;
	if ((currentSize + requested > mySize) || requested < 1)
		return NULL;
	switch (myStrategy) {
		case NotSet: 
			return NULL;
		case First:
			ptr = head;
			while(ptr != NULL) {
				if (ptr->alloc == 0 && ptr->size >= requested) {
					size_t nextSize = ptr->size - requested;
					ptr->alloc = 1;
					ptr->size = requested;
					insertAfter(ptr,nextSize);
					return ptr->ptr;
				}
				ptr = ptr->next;
			}
		case Best:
	 		return NULL;
		case Worst:
			return NULL;
		case Next:
	        return NULL;
	  }
	return NULL;
}


/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block)
{
	struct memoryList *temp;
	temp = head;
	while (temp != NULL) {
		if (temp->ptr == block) {
			temp->alloc = 0;
			currentSize -= temp->size;
			/*fix memory holes*/
			while (temp->last->alloc == 0 || temp->next->alloc == 0) {
				if (temp->last->alloc == 0) {
					struct memoryList *deadNode;
					temp->last->size += temp->size;
					temp->next->last = temp->last;
					temp->last->next = temp->next;
					deadNode = temp;
					temp = temp->last;
					free(deadNode);
				}
				if (temp->next->alloc == 0) {
					struct memoryList *deadNode;
					deadNode = temp->next;
					temp->next = deadNode->next;
					temp->size += deadNode->size;
					deadNode->next->last = temp;
					free(deadNode);
				}
			}
			break;
		} else
			temp = temp->next;
	}
	return;
}

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when we refer to "memory" here, we mean the 
 * memory pool this module manages via initmem/mymalloc/myfree. 
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes()
{
	int count = 0;
	struct memoryList *ptr = head;
	while (ptr != NULL) {
		if (ptr->alloc == 0)
			count++;
	}
	return count;
}

/* Get the number of bytes allocated */
int mem_allocated()
{
	return currentSize;
}

/* Number of non-allocated bytes */
int mem_free()
{
	return mySize - currentSize;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{
	size_t max = 0;
	struct memoryList *ptr = head;
	while (ptr != NULL) {
		if (ptr->size > max)
			max = ptr->size;
	}
	return (int)max;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size)
{
	size_t min = mySize;
	struct memoryList *ptr = head;
	while (ptr != NULL) {
		if (ptr->size < min)
			min = ptr->size;
	}
	return (int)min;
}       
//Always assumes ptr is valid
char mem_is_alloc(void *ptr)
{
	struct memoryList *temp = head;
	while (temp != NULL) {
		if (ptr >= temp->ptr && ptr < (temp->ptr + temp->size)) { //ptr lies within this block
			return temp->alloc;
		}
	}
	printf("*****ERROR");
        return 0;
}

/* 
 * Feel free to use these functions, but do not modify them.  
 * The test code uses them, but you may ind them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool()
{
	return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total()
{
	return mySize;
}


// Get string name for a strategy. 
char *strategy_name(strategies strategy)
{
	switch (strategy)
	{
		case Best:
			return "best";
		case Worst:
			return "worst";
		case First:
			return "first";
		case Next:
			return "next";
		default:
			return "unknown";
	}
}

// Get strategy from name.
strategies strategyFromString(char * strategy)
{
	if (!strcmp(strategy,"best"))
	{
		return Best;
	}
	else if (!strcmp(strategy,"worst"))
	{
		return Worst;
	}
	else if (!strcmp(strategy,"first"))
	{
		return First;
	}
	else if (!strcmp(strategy,"next"))
	{
		return Next;
	}
	else
	{
		return 0;
	}
}


/* 
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

/* Use this function to print out the current contents of memory. */
void print_memory()
{

	return;
}

/* Use this function to track memory allocation performance.  
 * This function does not depend on your implementation, 
 * but on the functions you wrote above.
 */ 
void print_memory_status()
{
	printf("%d out of %d bytes allocated.\n",mem_allocated(),mem_total());
	printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
	printf("Average hole size is %f.\n\n",((float)mem_free())/mem_holes());
}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv) {
        strategies strat;
	void *a, *b, *c, *d, *e;
	if(argc > 1)
	  strat = strategyFromString(argv[1]);
	else
	  strat = First;
	
	
	/* A simple example.  
	   Each algorithm should produce a different layout. */
	
	initmem(strat,500);
	
	a = mymalloc(100);
	b = mymalloc(100);
	c = mymalloc(100);
	myfree(b);
	d = mymalloc(50);
	myfree(a);
	e = mymalloc(25);
	
	print_memory();
	print_memory_status();
	
}
