/*
 * Memory-management functions
 *
 *
 * Copyright (c) 2013, Jonathan Lewis, Pavlo Bazilinskyy <pavlo.bazilinskyy@gmail.com>
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


#ifndef DALSMALL
static char *file_name = "DAL_mem"; // used by DAL_error macro
#endif

struct threadSafeList* mallocList; // List of memory allocated with calls of malloc()

// mutex to serialise memory operation when using a shared heap
my_mutex_t shared_heap_mutex;


/*
 * Assigns a memory address pntr to a variable of "pointer type" 
 * generic_var_pntr which is passed by reference to the function.
 */

void DAL_assign(void *generic_var_pntr, void *new_mem){
	PRINTF("DAL_assign\n");
	void **var_pntr = (void **) generic_var_pntr;
	void *old_mem = *var_pntr; // can deref void** but not void*
	//printf("assign: old_mem=%p, new_mem=%p\n", old_mem, new_mem);
	// if old memory address is same as the new one don't do anything
	if(old_mem == new_mem) return;
	// Otherwise if old memory is not null update ref_count and free if 0
	if(old_mem != NULL)
		DAL_decRef(old_mem);
	// if new memory is not null
	if(new_mem != NULL)
		DAL_incRef(new_mem);
	// complete the assignment of memory address to caller's variable
	*var_pntr = new_mem; // as var_pntr has type void** can dereference it here
}


void *DAL_alloc_in_specific_heap(size_t size, bool mem_contains_pointers, struct shMapType *heap){
	void* pntr = NULL;
#if HEAPS == HEAP_PRIVATE // Private heaps
	if(heap == NULL){
		heap = getCurrentPrivateHeapInfo();
	}
	pntr = (void *) SH_alloc_in_specific_heap(size + sizeof(MemHeaderStruct), heap); // Allocate into the private heap for the current thread.
#else // Shared Heap
	pntr = (void *) BASE_mem_alloc( size + sizeof(MemHeaderStruct) );	// Allocate memory into the big heap.
	listAdd(mallocList, pntr); // Note down that memory was allocated with malloc().
	log_into_file("malloc     at", pntr, size + sizeof(MemHeaderStruct)); // Log into a file, if required.
#endif

	// zero memory area to avoid having to set all pointer types to NULL
	// prior to DAL_assign
	memset(pntr, 0, (size + sizeof(MemHeaderStruct)) );
	MemHeader header = ((MemHeader) pntr);
	header->ref_count = 0;
	header->mem_contains_pointers = mem_contains_pointers;
	if(heap) header->mutex = &heap->mutex;
	else header->mutex = &shared_heap_mutex;
	return (pntr + sizeof(MemHeaderStruct));
}


void *DAL_alloc(size_t size, bool mem_contains_pointers){
	return DAL_alloc_in_specific_heap(size, mem_contains_pointers, NULL);
}

/*
 * Decrements reference count for memory referenced by pntr.
 */
void DAL_decRef(void *pntr) {
	PRINTF("DAL_decRef\n");

	if(pntr==NULL){
		//DAL_error(NULL_POINTER_ERROR);
		return;
	}
	MemHeader header = (MemHeader) (pntr - sizeof(MemHeaderStruct));
	my_mutex_lock(header->mutex);
	PRINTF("DAL_decRef: ref count = %d\n", header->ref_count);
	header->ref_count -= 1;

	if(header->ref_count <= 0) {
		if(header->mem_contains_pointers){    // memory contains pointers
			PointerContainerPNTR thisMem = (PointerContainerPNTR) pntr;
			thisMem->decRef(pntr);  // call decRef function
			PRINTF("Decref: garbage collect with pointers\n");
		} else { // memory is neither component nor contains pointers
			PRINTF("Decref: garbage collect with pointers\n");
		}
		my_mutex_unlock(header->mutex);
		DAL_free(pntr);         // free resources
	} else {// ref count is greater 0 after decrement, do nothing
		PRINTF("DAL_decRef: do nothing\n");
		my_mutex_unlock(header->mutex);
	}
}

/*
 * Modify reference count for memory referenced by pntr.
 */
void DAL_modRef_by_n(void *pntr, int n) {
	if(pntr==NULL){
		DAL_error(NULL_POINTER_ERROR);
		return;
	}
	MemHeader header = ((MemHeader)(pntr - sizeof(MemHeaderStruct)));
	my_mutex_lock(header->mutex);
	header->ref_count += n;
	my_mutex_unlock(header->mutex);
}

/*
 * Increments reference count for memory referenced by pntr.
 */
void DAL_incRef(void *pntr) {
	PRINTF("DAL_incRef\n");
	DAL_modRef_by_n(pntr, 1);
}

/*
 * Frees memory assigned by DAL_alloc and referenced by pntr.
 */
void DAL_free(void *pntr){

	PRINTF("DAL_free\n");
	if(pntr==NULL){
		DAL_error(NULL_POINTER_ERROR);
		return;
	}
	MemHeader header = (MemHeader) (pntr - sizeof(MemHeaderStruct));
	if(header->ref_count > 0){
		DAL_error(STILL_REFERENCED_ERROR);
		return;
	}
#if  DEBUG == 2
	printf("\nf(%p)\n", header);
#endif

	BASE_mem_free(header);
	PRINTF("\nf(%p)\n", header);
}
