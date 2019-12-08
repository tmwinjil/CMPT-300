LMP2: Memory Management
=======================

This machine problem will focus on memory.  You will implement your own
version of malloc() and free(), using a variety of allocation strategies.

You will be implementing a memory manager for a block of memory.  You will
implement routines for allocating and deallocating memory, and keeping track of
what memory is in use.  You will implement four strategies for selecting in
which block to place a new requested memory black:

  1) First-fit: select the first suitable block with smallest address.
  2) Best-fit: select the smallest suitable block.
  3) Worst-fit: select the largest suitable block.
  4) Next-fit: select the first suitable block after
     the last block allocated (with wraparound
     from end to beginning).


Here, "suitable" means "free, and large enough to fit the new data".

Here are the functions you will need to implement:

initmem():
  Initialize memory structures.

mymalloc():
  Like malloc(), this allocates a new block of memory.

myfree():
  Like free(), this deallocates a block of memory.

mem_holes():
  How many free blocks are in memory?

mem_allocated():
  How much memory is currently allocated?

mem_free():
  How much memory is NOT allocated?

mem_largest_free():
  How large is the largest free block?

mem_small_free():
  How many small unallocated blocks are currently in memory?

mem_is_alloc():
  Is a particular byte allocated or not?

We have given you a structure to use to implement these functions.  It is a
doubly-linked list of blocks in memory (both allocated and free blocks).  Every
malloc and free can create new blocks, or combine existing blocks.  You may
modify this structure, or even use a different one entirely.  However, do not
change function prototypes or files other than mymem.c.

IMPORTANT NOTE: Regardless of how you implement memory management, make sure
that there are no adjacent free blocks.  Any such blocks should be merged into
one large block.

We have also given you a few functions to help you monitor what happens when you
call your functions.  Most important is the try_mymem() function.  If you run
your code with "mem -try <args>", it will call this function, which you can use
to demonstrate the effects of your memory operations.  These functions have no
effect on test code, so use them to your advantage.

Running your code:

After running "make", run

1) "mem" to see the available tests and strategies.
2) "mem -test <test> <strategy>" to test your code with our tests.
3) "mem -try <args>" to run your code with your own tests
   (the try_mymem function).

You can also use "make test" and "make stage1-test" for testing.  "make
stage1-test" only runs the tests relevant to stage 1.

As in previous MPs, running "mem -test -f0 ..." will allow tests to run even
after previous tests have failed.  Similarly, using "all" for a test or strategy
name runs all of the tests or strategies.  Note that if "all" is selected as the
strategy, the 4 tests are shown as one.

One of the tests, "stress", runs an assortment of randomized tests on each
strategy.  The results of the tests are placed in "tests.out" .  You may want to
view this file to see the relative performance of each strategy.


Stage 1
-------

Implement all the above functions, for the first-fit strategy.  Use "mem -test
all first" to test your implementation.


Stage 2
-------

A) Implement the other three strategies: worst-fit, best-fit, and next-fit.  The
strategy is passed to initmem(), and stored in the global variable "myStrategy".
Some of your functions will need to check this variable to implement the
correct strategy.

You can test your code with "mem -test all worst", etc., or test all 4 together
with "mem -test all all".  The latter command does not test the strategies
separately; your code passes the test only if all four strategies pass.


Questions
=========

1) Why is it so important that adjacent free blocks not be left as such?  What
would happen if they were permitted?

Our memoryList is tracking an actual contiguous region of memory myMemory. 2 adjacent block of memory in the list both represent the same region of 
unallocated memory in myMemory. If for example we had a 10byte block and 40 byte block adjacent in memoryList and wanted to 
allocate 50bytes then the memoryList would say there is no space, even though we have enough space in myMemory to allocate it.

2) Which function(s) need to be concerned about adjacent free blocks?

-myMalloc(), because adjacent free block should still be considered as one memory space.
-mem_holes(), because 2 holes adjacent to one another are still the same hole.
-myFree(), since we do not want tto create 2 free adjacent blocks upon freeing.

3) Name one advantage of each strategy.

First:	very little overhead.
best:	better memory utilization than the first because it searches the smallest free partitioned block
worst:	reduces the rate of creating smaller gap of free blocks
next:	similar to the first, but uses less searching (and is thus faster) because it starts where it left off. e.g in a case of continuous allocations and no free, 
	it would always find the next space in 1 step whereas first will take longer and longer the more space we allocate.

Source: https://www.tutorialspoint.com/operating_system/os_memory_allocation_qa2.htm

4) Run the stress test on all strategies, and look at the results (tests.out).
What is the significance of "Average largest free block"?  Which strategy
generally has the best performance in this metric?  Why do you think this is?

It shows that at any point what size of block we are expecting. We believe that the best-fit has the best 
performace in "average largest free block". This is because the best-fit will always try to maximize the 
smaller block rather than breaking up the largest block.

5) In the stress test results (see Question 4), what is the significance of
"Average number of small blocks"?  Which strategy generally has the best
performance in this metric?  Why do you think this is?

Number of possibly unallocatable blocks of memory. So more the smaller block we have, more fragmenation we have,
thus, less likely to find another memory space. We believe that the worst-fit has best performance (least number of small blocks)
in the "average number of small blocks", and we believe this is due to the strategy trying to break down the largest block, 
resulting in large memory space after allocating. 

6) Eventually, the many mallocs and frees produces many small blocks scattered
across the memory pool.  There may be enough space to allocate a new block, but
not in one place.  It is possible to compact the memory, so all the free blocks
are moved to one large free block.  How would you implement this in the system
you have built?

Yes it is possibe to compact the memory. Count the number of free blocks and their size. Add all the sizes, 
and then place them in the front of the buffer. It could be implemented
in the myFree function everytime the block is deallocated.

7) If you did implement memory compaction, what changes would you need to make
in how such a system is invoked (i.e. from a user's perspective)?

We would add an additional function for the user because it would take a while to do memory compaction. User
can decide to do so if they wish to optimize the memory buffer.

8) How would you use the system you have built to implement realloc?  (Brief
explanation; no code)

We check if there is free space around our block, that would fit the new size. If so, we would expand the block
into the free space. If not, we would unallocate the current memory space, and reallocate at the end of the space
if possible.

9) Which function(s) need to know which strategy is being used?  Briefly explain
why this/these and not others.

mymalloc, because the function requires how to select the next block to be allocated. 

10) Give one advantage of implementing memory management using a linked list
over a bit array, where every bit tells whether its corresponding byte is
allocated.

Linked list is dynamic, so the buffer size can be increased during the allocation and deallocation of blocks, while
bit array is static.
