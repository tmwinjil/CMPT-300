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
	if (head) free(head)

	myMemory = malloc(sz);
	
	/* TODO: Initialize memory management structure. */
	head = malloc(sizeof(struct memoryList));
	head->size =sz;
	head->alloc = 0;
	head->ptr = myMemory;
	next = head;

	// implementation of circular linked-list
	head->last = head;
	head->next = head;

}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */

void *mymalloc(size_t requested)
{
	assert((int)myStrategy > 0);
	
	struct memoryList *block = NULL;
	switch (myStrategy){
		case First:
			block = first_block(requested);
			break;
		case Best:
	        block = best_block(requested);
			break;
		case Worst:
	        block = worst_block(requested);
			break;
		case Next:
	        block = next_block(requested);
			break;
		}
	default:
		printf("No strategies has been set. Try Again\n");
		return NULL;

	if (!block){
		printf("Not a valid block!\n")
		return NULL;
	}

	if (block->size > requested){
		struct memoryList* remainder = malloc(sizeof(struct memoryList));

		// insert into linked list
		remainder->next = block->next;
		reminader->next->last = remainder;
		raminder->last = block;
		block->next =remainder;

		// divide up the allocated memory
		remainder->size = block->(size - requested);
		reminader->alloc = 0;
		remainder->ptr = block->ptr + requested;
		block->size =requested;
		next = remainder;
	} else {
		next = block->next;
	}

	block > alloc = 1;
	
	return block->ptr;
}


/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block)
{
	struct memoryList* blockContainer = head;
	do{
		if (blockContainer->ptr == block)
			break;
	} while ((blockContainer = blockContainer->next) != head);

	blockContainer->alloc = 0;

	if ((blockContainer->last->alloc) &&(blockContainer != head)){
		struct memoryList* previousBlock = blockContainer->last;
		previousBlock->next = blockContainer->next;
		previousBlock->next->last = previousBlock;
		previousBlock->size += blockContainer->size;

		if (next == blockContainer){
			next = previousBlock;
		}

		free(blockContainer);
		blockContainer = prev;
	}

	if (!(blockContainer->next->alloc) && (blockContainer->next != head)){
		struct memoryList* secondBlock = blockContainer->next;
		blockContainer->next = secondBlock->next;
		blockContainer->next->last = blockContainer;
		blockContainer->size += secondBlock->size;

		if (next == secondBlock){
			next = blockContainer;
		}

		free(secondBlock);
	}
	return;
}

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when we refer to "memory" here, we mean the 
 * memory pool this module manages via initmem/mymalloc/myfree. 
 */


struct memoryList* first_block(size_t requested)
{
	struct memoryList *index = head;

	do {
		if(!index->alloc) && index->size >= requested){
			return index;
		}
	} while((index = index->next) != head);

	return NULL;
}

struct memoryList* best_block(size_t requested)
{
	struct memoryList* index = head, *smallestBlock = NULL;

	do {
		if (!(index->alloc) && (!smallestBlock || index->size < smallestBlock->size) && index->size >= requested)
			smallestBlock = index;
	} while((index = index->next) != head);

	return smallestBlock;
}

struct memoryList* worst_block(size_t requested)
{
	struct memoryList* index = head, *largestBlock = NULL;

	do {
		if(!(index->alloc) && (!largestBlock || index->size > largestBlock->size))
			largestBlock = index;
	} while ((index = index->next) != head);

	if (largestBlock->size >= requested)
		return largestBlock;
	else 
		return NULL;
}

struct memoryList* next_block(size_t requested)
{
	struct memoryList* startingBlock = next;

	do {
		if((next->size >= requested) && !(next->alloc))
			return next;
	} while ((next = next->next) != startingBlock);

	return NULL;
}
/* Get the number of contiguous areas of free space in memory. */
int mem_holes()
{
	return mem_small_free(mySize + 1);
}

/* Get the number of bytes allocated */
int mem_allocated()
{
	return mySize - mem_free();
}

/* Number of non-allocated bytes */
int mem_free()
{
	int count = 0;

	struct memoryList* index = head;
	do {
		if(!(index->alloc)){
			count += index->size;
		}
	} while ((index = index->next) != head);

	return count;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{
	int maxSize = 0;

	struct memoryList* index = head;
	do {
		if ((index->size > maxSize) && !(index->alloc))
			maxSize = index->size;
	} while ((index = Index->next) != head);

	return maxSize;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size)
{
	int countBlocks = 0;

	struct memoryList* index = head;
	do {
		if(index->size <= size)
			countBlocks += !(index->alloc);
	} while ((index = index->next) != head);

	return countBlocks;
}       

char mem_is_alloc(void *ptr)
{
	struct memoryList* index = head;
	while(index->next != head){
		if (ptr < index->next->ptr){
			return index->alloc;
		}
		index = index->next;
	}

    return index->alloc;
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
