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


#ifndef COMPONENT_H
#define COMPONENT_H
#endif

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <insense/ThreadSafeList.h>
#include <insense/GlobalVars.h>
#include <insense/DAL_mem.h>
#include <insense/affinity.h>
#include <insense/Bool.h>
#include <insense/my_semaphore.h>

#if HEAPS == HEAP_PRIVATE
extern pthread_mutex_t thread_lock;		// Mutex for avoiding issues with scheduling of Insense components.
#endif

typedef void (*behaviour_ft)(void *, int, void **);

typedef struct IComponent_data *IComponentPNTR, IComponentStruct;



struct IComponent_data {	        		// The supertype for all components
	void (*decRef)(void *pntr);           	// for ref counting garbage collection
	bool stopped;                         	// Has the component been stopped
	pthread_t behav_thread;         		// Process implementing behaviour, runs on a POSIX thread
	my_sem_t component_create_sem;			// Semaphore for component_create
};

/*
 * Structure used for a wrapper function used in pthread_create.
 * Hint from: http://stackoverflow.com/questions/8976419/passing-more-than-one-parameter-to-pthread-create
 */
struct argStructType {
	int argc;
	void ** argv;
	behaviour_ft behaviour;
	void * this_ptr;
};

/*
 * Wrapper to be used in pthread_create function
 */
void * startRoutine(void * args);

/*
 * Will create a new component.
 *
 *
 * THIS_PTR *ptr = compoenet_create();
 *
 *
 * request component referenced by this_ptr to terminate itself the next time it
 * tests its loop condition
 *
 *
 * int core - ID of a core for manually setting affinity. -1 by default: affinity should be set manually.
 */
void *component_create(behaviour_ft behaviour, int struct_size, int stack_size, int argc, void *argv[], int core);

/*
 * Will casue the termination of a componenet whos 'this' pointer is passsed in.
 *
 * this_ptr: the returned pointer from the component_create call who you
 * wish to terminate.
 *
 * THIS_PTR *ptr = component_create();
 *
 * component_stop(ptr);
 *
 *
 * request component referenced by this_ptr to terminate itself the next time it
 * tests its loop condition
 */
void component_stop(void *this_ptr);

/*
 * currently running component terminates itself
 * there is no return; after destroying the calling
 * component, the scheduler schedules ready to run component.
 * Destruction will also tidy up any channels associated with the component

 */
void component_exit(void);

/**
 *              Relinquish control of the processor
 *
 *              This function declares that a component wants to give up control
 *              of the proccessor.
 *
 *              NOTE you may only need either a blank function here (if you want to avoid modifying the compiler)
 *              or you can leave this out and change compiler so that it doesn't make calls to yield.
 */
void component_yield(void);

/*
 * Initial function that will act as the behaviour for the primordial component.
 *
 * This function will be the first behaviour run in the system.
 *
 * It is expected that this function will be used for the component creation
 * as well as initial channel bindings and will be defined in the user code
 *
 */
void primordial_main(void *this);
