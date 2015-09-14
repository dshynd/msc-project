/*
 * Memory management function declarations
 *
 *
 * Copyright (c) 2013, Jonathan Lewis, Alan Dearle, Pavlo Bazilinskyy <pavlo.bazilinskyy@gmail.com>
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

#ifndef DAL_MEM_H_
#define DAL_MEM_H_

#include <stdlib.h>
#include <stdio.h>
#include <insense/Bool.h>
#include <insense/GlobalVars.h>
#include <pthread.h>
#include <time.h>
#include <insense/ThreadSafeList.h>
#include <insense/DAL_error.h>
#include <string.h>
#include <insense/strmap.h>
#include <sys/mman.h>
#include "my_mutex.h"

// added and used by jonl in place of 0, 1
#define HEAP_SHARED 0
#define HEAP_PRIVATE 1
// end added by jonl


extern pthread_t mainThread; // ID of the main thread.

// Choose which memory allocator to use (shared heap / private heaps)
#if HEAPS == HEAP_PRIVATE
#define BASE_mem_alloc SH_alloc
#define BASE_mem_free SH_free
#else
#define BASE_mem_alloc BH_alloc
#define BASE_mem_free BH_free
#endif

typedef void (*decRefFunc_t)(void *pntr);

// mutex to serialise memory operation when using a shared heap
extern my_mutex_t shared_heap_mutex;

typedef struct MemHeader {
	unsigned long ref_count; // 64-bit architecture
	unsigned short mem_contains_pointers;
	my_mutex_t *mutex;       // pntr to mutex to serialise memory operations
}*MemHeader, MemHeaderStruct;

/*
 * Structure used for passing pthread_t and memory address.
 */
struct shMapType {
	pthread_t thread_id;		// ID of the thread that is linked to the small heap
	unsigned int * memArea;		// The actual memory range used for allocating memory within the small heap
	unsigned int memAreaSize;	// Size of the small heap
	unsigned int available;		// How much memory is available for allocation
	my_mutex_t mutex;           // mutex to serialise memory operations for private heaps
};

/*
 * A memory header for objects dynamically allocated in private heaps
 * This struct stores the ID of the thread that allocated memory for an object and can be used to
 * find out information about the private heap containing the object by locating the appropriate shMapType entry.
 * This info is needed when memory is allocated in one thread and freed by another which happens when sending
 * complex data types between components.
 */
typedef struct SHMemHeader {
	pthread_t thread_id;		// ID of the thread that allocated the memory
}*SHMemHeader, SHMemHeaderStruct;


// Every object that contains pointers has a pntr to a function decRef that 
// decrements the references for memory refernced by its pointer fields
typedef struct PointerContainer {
	decRefFunc_t decRef;
} PointerContainerStruct, *PointerContainerPNTR;

// CopyableObject logically extends PointerContainer
// Every object that can be copied has a reference to a copy function
typedef struct CopyableObject {
	decRefFunc_t decRef;
	void *(*copyObject)(void *this, bool);
} CopyableObjectStruct, *CopyableObjectPNTR;

extern void DAL_assign(void *generic_var_pntr, void *new_mem);
extern void *DAL_alloc(size_t size, bool contains_pointers);
extern void *DAL_alloc_in_specific_heap(size_t size, bool mem_contains_pointers, struct shMapType *shMapEntry);
extern unsigned DAL_getRef(void *pntr);
extern bool DAL_mem_contains_pointers(void *pntr);
extern void DAL_mem_set_contains_pointers(void *pntr, bool mem_contains_pointers);
extern void DAL_decRef(void *pntr);
extern void DAL_incRef(void *pntr);
extern void DAL_modRef_by_n(void *pntr, int n);
extern void DAL_free(void *pntr);

// Used with private heaps
extern struct shMapType *getCurrentPrivateHeapInfo(void);
extern struct shMapType *new_PrivateHeap(void);
extern void *SH_create_small_heap(void);
extern void *SH_alloc_in_specific_heap(unsigned size, struct shMapType * shMapEntry);
extern void *SH_alloc(unsigned size);
extern void *SH_alloc_at_base(unsigned int size, struct shMapType * shMapEntry);
extern void SH_free(void *pntr);
struct shMapType * SH_init(void *ptr, pthread_t thread);
void * log_into_file(char * string, void * ptr, unsigned int size);

// Used with the big heap
extern void *BH_alloc(unsigned int size);
extern void BH_free(void *pntr);

#endif /*DAL_MEM_H_*/
