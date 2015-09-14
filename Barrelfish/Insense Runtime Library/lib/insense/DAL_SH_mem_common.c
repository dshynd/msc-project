/*
 * Memory management functions used with small heap memory allocation scheme.
 * Based on work by http://stackoverflow.com/users/626431/mnicky
 *
 * Copyright (c) 2013, Pavlo Bazilinskyy <pavlo.bazilinskyy@gmail.com>
 * School of Computer Science, St. Andrews University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <insense/DAL_mem.h>

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
#ifndef NULL
#define NULL   ((void *) 0)
#endif

unsigned heapSize = 4000980;		// Size (in bytes) of one small heap.
pthread_t mainThread;				// ID of the main thread.
struct threadSafeList* mallocList;	// List of memory allocated with calls of malloc()

#if DEBUG 
static void *last_max_malloc;
#endif

#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#define CHAN_PRINT(...) channel_print(__VA_ARGS__)
#else
#define PRINTF(...)
#define CHAN_PRINT(...)
#endif

#if MCDEBUG
#define PRINTFMC(...) printf(__VA_ARGS__)
#else
#define PRINTFMC(...)
#endif

/*
 * Get size of one block.
 */
static unsigned int blockSize(unsigned int x, unsigned int *mem) {
	return mem[x];
}

/*
 * Get a reference to the next block.
 */
static unsigned int refToNextBlock(unsigned int x, unsigned int *mem) {
	return x + mem[x];
}

/*
 * Get the nest block in the memory.
 */
static unsigned int nextBlock(unsigned int x, unsigned int *mem) {
	return mem[x + mem[x]];
}

/*
 * Initialise the small heap.
 * Parameters that are allocated inside of this functions are described in the header file.
 */
struct shMapType *SH_init(void *ptr, pthread_t thread) {
	struct shMapType * shMapElement = malloc(sizeof(struct shMapType));
	shMapElement->memArea = (unsigned int *) ptr;
	shMapElement->memAreaSize = heapSize / sizeof(unsigned int);
	shMapElement->memArea[0] = heapSize / sizeof(unsigned int) - 1;
	shMapElement->memArea[shMapElement->memAreaSize - 1] = heapSize / sizeof(unsigned int);
	shMapElement->thread_id = thread;
	shMapElement->available = 0;
	my_mutex_init(&shMapElement->mutex);
	PRINTFMC("Component created. Heap location: %p\n", shMapElement->memArea);
	return shMapElement;
}

/*
 * Allocator of memory, allocates into the specified small heap
 */
void *SH_alloc_in_specific_heap(unsigned size, struct shMapType * shMapEntry){
	if (size == 0) {  //return NULL pointer after attempt to allocate 0-length memory
		return NULL ;
	}
	if (shMapEntry == NULL ) {
		if ( pthread_self() == mainThread ) {
			// If it is the main thread - malloc space
			void * result = (void *) malloc(size + sizeof(MemHeaderStruct));
			listAdd(mallocList, result);
			log_into_file("malloc     at", result, size + sizeof(MemHeaderStruct));  // Log into a file, if required.
			return result;
		}
		PRINTFMC("\nSize of SHList %d. Pthread_self %u. NULL returned from the list.", SHList->count, (unsigned) pthread_self());
		// If nothing can be done - return NULL
		return NULL ;
	} else {
		return SH_alloc_at_base(size, shMapEntry);
	}
}

// Get hold of info for the current thread's private heap
struct shMapType *getCurrentPrivateHeapInfo(void){
	struct shMapType *shMapEntry = listGetMemoryLocation(SHList, pthread_self());
	return shMapEntry;
}


/*
 * Allocator of memory, allocates into a small heap linked to the current thread.
 * Given the base of the heap.
 */
void * SH_alloc(unsigned size){
	struct shMapType *shMapEntry = getCurrentPrivateHeapInfo();
	return SH_alloc_in_specific_heap(size, shMapEntry);
}

/*
 * Allocator of memory, allocates into the specified small heap
 */
void *SH_alloc_at_base(unsigned int data_size, struct shMapType * shMapEntry) {
	if (data_size == 0) {  // Return NULL pointer after attempt to allocate 0-length memory
		return NULL ;
	}

	my_mutex_lock(&shMapEntry->mutex);

	// changed to deal with sh memory header containing the allocator's thread_id
	unsigned int size = data_size + sizeof(SHMemHeaderStruct);
	unsigned int num = size / sizeof(unsigned int);
	if (size % sizeof(unsigned int) > 0)
		num++;
	unsigned int cur, prev;  					// Index of the current block, previous block
	unsigned int isFirstFreeBeingAllocated = 1; // Whether the first free block is being allocated

	prev = cur = shMapEntry->available;

	// Check if we have enough free space for allocation
	test:

	if (shMapEntry->available == shMapEntry->memAreaSize) {  // If we are on the end of the memory
		return NULL ;
	}

	if (blockSize(cur, shMapEntry->memArea) < num) {  // If the size of free block is lower than requested
		isFirstFreeBeingAllocated = 0;
		prev = cur;

		if (nextBlock(cur, shMapEntry->memArea) == shMapEntry->memAreaSize) {  // If not enough memory
			return NULL ;
		} else
			cur = nextBlock(cur, shMapEntry->memArea);
		goto test;
	}

	if (blockSize(cur, shMapEntry->memArea) == num) {  // If the size of free block is equal to requested

		if (isFirstFreeBeingAllocated)
			shMapEntry->available = nextBlock(cur, shMapEntry->memArea);
		else
			shMapEntry->memArea[refToNextBlock(prev, shMapEntry->memArea)] = nextBlock(cur, shMapEntry->memArea);
	}

	else {  // If the size of free block is greater than requested

		if (isFirstFreeBeingAllocated) {
			if ((blockSize(cur, shMapEntry->memArea) - num) == 1)  // If there is only 1 free item left from this (previously) free block
				shMapEntry->available = nextBlock(cur, shMapEntry->memArea);
			else
				shMapEntry->available = cur + num + 1;
		} else {
			if ((blockSize(cur, shMapEntry->memArea) - num) == 1)  // If there is only 1 free item left from this (previously) free block
				shMapEntry->memArea[refToNextBlock(prev, shMapEntry->memArea)] = nextBlock(cur, shMapEntry->memArea);
			else
				shMapEntry->memArea[refToNextBlock(prev, shMapEntry->memArea)] = cur + num + 1;
		}

		if ((blockSize(cur, shMapEntry->memArea) - num) == 1)  // If there is only 1 free item left from this (previously) free block
			shMapEntry->memArea[cur] = num + 1;
		else {
			shMapEntry->memArea[cur + num + 1] = blockSize(cur, shMapEntry->memArea) - num - 1;
			shMapEntry->memArea[cur] = num;
		}
	}
	log_into_file("allocated at", (void *) &(shMapEntry->memArea[cur + 1]), size);  // Log into a file, if required.
	// return (void *) &(shMapEntry->memArea[cur + 1]);
	// change to deal with mem header recording the allocator's thread_id
	char *new_base = (char *) &(shMapEntry->memArea[cur + 1]);
	SHMemHeader header = (SHMemHeader) new_base;
	header->thread_id = shMapEntry->thread_id;
	my_mutex_unlock(&shMapEntry->mutex);
	return (void *) ( new_base + sizeof(SHMemHeaderStruct));
}

/*
 * Freeing memory from the small heap
 */
void SH_free(void *mem) {
	//First check if memory was allocated using malloc()
	void * adr = listGetMallocedMemoryAdr(mallocList, mem);
	if (adr != NULL ) {
		free(adr); // Memory was allocated using malloc(), use free instead
		listRemove(mallocList, adr); // Remove address from the list
		return;
	}

	// Memory was not allocated by using malloc, find which small heap it was put into.
	//struct shMapType * shMapEntry = listGetMemoryLocation(SHList, pthread_self());
	// changed to extract allocator's thread_id from mem header rather than by using pthread_self()
	void *ptr = mem - sizeof(SHMemHeaderStruct);
	SHMemHeader header = (SHMemHeader) ptr;
	struct shMapType * shMapEntry = listGetMemoryLocation(SHList, header->thread_id);

	my_mutex_lock(&shMapEntry->mutex);
	unsigned int toFree;  // Pointer to block that needs to be freed
	unsigned int cur, prev;

	toFree = ((unsigned int *) ptr - (shMapEntry->memArea + 1));

	if (toFree < shMapEntry->available) {  // If block, that is being freed is before the first free block

		if (((refToNextBlock(toFree, shMapEntry->memArea) + 1) == shMapEntry->available) && shMapEntry->available < shMapEntry->memAreaSize) // If next free block is immediately after block that is being freed
			shMapEntry->memArea[toFree] += (shMapEntry->memArea[shMapEntry->available] + 1);  // Defragmentation of free space
		else
			shMapEntry->memArea[refToNextBlock(toFree, shMapEntry->memArea)] = shMapEntry->available;

		shMapEntry->available = toFree;
	}

	else {  // If block, that is being freed isn't before the first free block

		prev = cur = shMapEntry->available;

		while (cur < toFree) {
			prev = cur;
			cur = nextBlock(cur, shMapEntry->memArea);
		}

		if ((refToNextBlock(prev, shMapEntry->memArea) + 1) == toFree) { // If previous free block is immediately before block that is being freed

			shMapEntry->memArea[prev] += (shMapEntry->memArea[toFree] + 1);  // Defragmentation of free space

			if (((refToNextBlock(toFree, shMapEntry->memArea) + 1) == cur) && cur < shMapEntry->memAreaSize) // If next free block is immediately after block that is being freed
				shMapEntry->memArea[prev] += (shMapEntry->memArea[cur] + 1);  // Defragmentation of free space
			else
				shMapEntry->memArea[refToNextBlock(toFree, shMapEntry->memArea)] = cur;
		} else {
			shMapEntry->memArea[refToNextBlock(prev, shMapEntry->memArea)] = toFree;
			shMapEntry->memArea[refToNextBlock(toFree, shMapEntry->memArea)] = cur;
		}

	}
	// jonl changed following line as NULL is incorrect, however, it is unclear what it shouldbe
	//log_into_file("freed from", ptr, NULL );
	log_into_file("freed from", ptr, 0 );
	my_mutex_unlock(&shMapEntry->mutex);
}


struct shMapType *new_PrivateHeap(void){
	void *new_heap = SH_create_small_heap();
	struct shMapType *new_heapmap_element = SH_init(new_heap, NULL ); 		// Create a new element to be put to the map
	return new_heapmap_element;
}

/*
 * Create a new small heap in main memory.
 */
void *SH_create_small_heap(void) {
	// Small heap has not been allocated to the thread yet
	void * heap = mmap(NULL, heapSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0); // Use mmap to allocate space for the new small heap.
	if (heap == MAP_FAILED)
		PRINTFMC("MMAP failed");
	return heap;
}

/*
 * Log information about allocated and freed memory into a file.
 * Used for debugging.
 */
void * log_into_file(char * string, void * ptr, unsigned int size) {
//	FILE *fp;
//
//	/* open the file */
//	fp = fopen("SH_alloc_log.txt", "a");
//	if (fp == NULL) {
//	 printf("I couldn't open results.dat for appending.\n");
//	 exit(0);
//	}
//
//	/* write to the file */
//	if ( (void *) size != NULL)
//		fprintf(fp, "%d: %u - %s %p - %uB\n", (int)time(NULL), (unsigned) pthread_self(), string, ptr, size);
//	else
//		fprintf(fp, "%d: %u - %s   %p \n", (int)time(NULL), (unsigned) pthread_self(), string, ptr);
//
//	/* close the file */
//	fclose(fp);
//
	return 0;
}
