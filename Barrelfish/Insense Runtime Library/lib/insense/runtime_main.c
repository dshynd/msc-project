/*
 * Main file for the runtime.
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

#include <insense/GlobalVars.h>
#include <insense/ThreadSafeList.h>
#include <insense/component.h>
#include <insense/DAL_mem.h>
#include <pthread.h>
#include <insense/cache_line_size.h>
#include <time.h>
#include <sys/time.h>
#include <inttypes.h>

#include <barrelfish/barrelfish.h>
#include <barrelfish/sys_debug.h>
#include <posixcompat.h>

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
#ifndef NULL
#define NULL   ((void *) 0)
#endif

#if MCDEBUG
#include <stdio.h>
#define PRINTFMC(...) printf(__VA_ARGS__)
#else
#define PRINTFMC(...)
#endif

pthread_t mainThread;
struct threadSafeList* threadList;
struct threadSafeList* SHList;
struct threadSafeList* mallocList;

int num_to_span = -1;

static int rrPlacement(enum pthread_action action, int coreid){
	static int current_core = -1;	
	if(action == PTHREAD_ACTION_CREATE){
		current_core = (current_core + 1) % num_to_span;
		return current_core;
	}

	return 0;
}

bool all_spanned = false;
static void span_cb(void *arg, errval_t err)
{
	if (err_is_fail(err)) {
		DEBUG_ERR(err, "span failed");
		return;
	}

	static int num_spanned = 1;

	num_spanned++;
	if (num_spanned >= num_to_span) {
		all_spanned = true;
	}
}

/*
 * Entry point of Multi-core Insense runtime.
 */
int main(int argc, char* argv[]) {
	PRINTFMC("Cache line size: %dB\n", cache_line_size());
	PRINTFMC("Main thread: %u\n", (unsigned) pthread_self());

	errval_t err;
	coreid_t mycore = disp_get_core_id();

	if (argc == 2) {
		num_to_span = atoi(argv[1]);
		if(num_to_span==0)
			all_spanned = true;		

		debug_printf("Spanning onto %d cores\n", num_to_span);
		for (int i = 1; i < num_to_span; i++) {
			err = domain_new_dispatcher(mycore + i, span_cb, NULL);
		    
			if (err_is_fail(err)) {
				DEBUG_ERR(err, "failed span %d", i);
			} 
		}
	} else {
		debug_printf("ERROR: Must specify number of cores to span\n");
		return EXIT_FAILURE;
	}

	posixcompat_pthread_set_placement_fn(rrPlacement);

	while (!all_spanned) {
		thread_yield();
	}

	my_mutex_init(&shared_heap_mutex);
#if HEAPS == HEAP_PRIVATE // Private heaps
	// Initialize mutex
	if (pthread_mutex_init(&thread_lock, NULL ) != 0) {
		PRINTF("Mutex initialization failed.\n");
		return -1;
	}
#endif

	mainThread = pthread_self(); // Note the ID of the main thread.

	// Create a list for storing references to p-threads
	threadList = listCreate();

	// Create map used to store memory locations of small heaps (using Thread safe list)
	SHList = listCreate();

	// Create map used to store memory locations what is allocated using malloc
	mallocList = listCreate();

// Start recording execution time
#if TIMING
	// CPU time
	uint64_t start, end;
	uint64_t tsc_per_ms = 0;
	sys_debug_get_tsc_per_ms(&tsc_per_ms);
	start = rdtsc();
#endif

	// Call primordial_main.
	primordial_main(NULL );

	// Join all p-threads
	if (threadList != NULL ) {
		listJoinThreads(threadList);
	}

// Stop recording execution time
#if TIMING
	end = rdtsc();
	
	uint64_t diff = (end - start) / tsc_per_ms;
	float elapsed = (diff / 1000) + ((diff % 1000) / 1000.0);

	printf("CPU:  %f seconds elapsed\n", elapsed);
#endif

	// Destroy lists and free memory
	listDestroy(threadList);
	listDestroy(SHList);
	listDestroy(mallocList);
#if HEAPS == HEAP_PRIVATE
	pthread_mutex_destroy(&thread_lock); 	// Destroy mutex lock used with pthreads
#endif
	return 0;
}
