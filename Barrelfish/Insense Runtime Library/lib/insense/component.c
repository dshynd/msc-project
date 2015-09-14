/*
 * Functions for working with components.
 *
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
#include <insense/component.h>

#if MCDEBUG
#define PRINTFMC(...) printf(__VA_ARGS__)
#else
#define PRINTFMC(...)
#endif

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
#ifndef NULL
#define NULL   ((void *) 0)
#endif

#if HEAPS == HEAP_PRIVATE
pthread_mutex_t thread_lock;		// Mutex for avoiding issues with scheduling of Insense components.
#endif

struct threadSafeList* threadList;	// List of threds.
struct threadSafeList* SHList;		// List of small heaps.

/*
 * Wrapper function for passing three arguments to a function that is to be run on the POSIX thread.
 */
void * startRoutine(void *args_p) { // this function executes at the start of the component thread
#if HEAPS == HEAP_PRIVATE
	pthread_mutex_lock(&thread_lock); 	// Wait until main thread has entered this thread's heap into the heap map
	pthread_mutex_unlock(&thread_lock); // Unlock
#endif
	struct argStructType *args = (struct argStructType *) args_p; 	// Unpack arguments
	args->behaviour(args->this_ptr, args->argc, args->argv); 		// Call the appropriate behaviour function with its arguments
	free(args_p);													// Free memory allocated for generic argument struct

	return ((void *) 1);
}

/*
 * Initialise an Insense component and start a POSIX thread running the behaviour function from the component.
 */
void *component_create(behaviour_ft behaviour, int struct_size, int stack_size, int argc, void *argv[], int core) {
	// Define thread
	struct IComponent_data *this_ptr;
	// Allocate space for the struct
#if HEAPS == HEAP_PRIVATE // Private heaps
	struct shMapType *heapElement = new_PrivateHeap(); 		// Create a new private heap (to be put to the heap map)
	this_ptr = DAL_alloc_in_specific_heap(struct_size, true, heapElement); 			// Allocate space for this_ptr in the newly created private heap
	if (this_ptr == NULL ) {
		return NULL ;
	} else {
		memset(this_ptr, 0, struct_size);
	}
#else // Shared heap
	if ((this_ptr = ((struct IComponent_data *) DAL_alloc(struct_size, true))) == NULL ) {
		return NULL;
	} else {
		memset(this_ptr, 0, struct_size);
	}
#endif
	// Initialize this->comp_create_sem
	my_sem_init(&(this_ptr->component_create_sem), 0);
	// Setup the stopped condition
	if (struct_size) {
		struct IComponent_data *t = (struct IComponent_data*) this_ptr;
		t->stopped = 0;
	}

	// Define new structure for arguments for the wrapper function
	// Whenever dealing with garbage collection, first define as NULL
	struct argStructType * argStruct = malloc(sizeof(struct argStructType));
	argStruct->behaviour = behaviour;
	argStruct->argc = argc;
	argStruct->argv = argv;
	argStruct->this_ptr = this_ptr;

	// Create thread
#if HEAPS == HEAP_PRIVATE // Private heaps
	pthread_mutex_lock(&thread_lock); // Lock mutex to make component thread wait until its heap has been put inserted into the heap map
#endif

	pthread_create(&this_ptr->behav_thread, NULL, startRoutine, argStruct); // Create a POSIX thread, use wrapper function startRoutine to pass three arguments to the function running inside of the thread

	//Set affinity
#if AFFINITY_ALGO != AFFINITY_DYNAMIC
	if (core != -1) { // Manually passed Core ID
		setAffinityToCore(this_ptr->behav_thread, core);// Use passed ID of a core
	} else { // Core ID was not passed to the component_create function
		setAffinity(this_ptr->behav_thread);// Use an algorithm defined in GlobalVars.h
	}
	getAffinityThread(this_ptr->behav_thread); // Check if setting affinity worked. Data outputted only in PRINTMC is defined.
#endif

	// If small heaps are used, add a new entry to the map with a pointer to a newly created pthread.
#if HEAPS == HEAP_PRIVATE // Private heaps
	heapElement->thread_id = this_ptr->behav_thread; // Put a thread id to the element to be to the map
	PRINTFMC("Component created. Thread ID: %x\n", heapElement->thread_id);
	listAdd(SHList, heapElement);
	pthread_mutex_unlock(&thread_lock); // Unlock mutex to permit component to continue now that its heap has been put inserted into the heap map
#endif
	// Insert thread into the list of threads
	listAdd(threadList, this_ptr->behav_thread);
	my_sem_wait(&this_ptr->component_create_sem); // Wait for creation of the component
	return this_ptr;
}

// Stop component
void component_stop(void * this_ptr) {
	//*((int*)this_ptr) = 1;
	struct IComponent_data *t = (struct IComponent_data*) this_ptr;
	t->stopped = 1;
}

// Empty yield function. Left here for compatibility with InceOS.
void component_yield(void) {
}

// Empty exit function. Left here for compatibility with InceOS.
void component_exit(void) {
}
