/*
 * Thread-safe linked-list.
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

#ifndef __THREADSAFELIST_H
#define __THREADSAFELIST_H

#include <pthread.h>
#include <insense/DAL_mem.h>

#ifndef NULL
#define NULL   ((void *) 0)
#endif

extern struct threadSafeList* threadList; 	// A list of p-threads.
extern struct threadSafeList* SHList; 		// A map for allocating memory with small heaps.
extern struct threadSafeList* mallocList; 	// A list of memory allocated with malloc().

/*
 * Structure for the item of the thread-safe list.
 */
struct listItem {
	void *value;
	struct listItem *prev;
	struct listItem *next;
};

/*
 * Structure for the thread-safe list.
 */
struct threadSafeList {
	int count; 			   // Number of elements in the list.
	struct listItem *head; // First element in the list.
	struct listItem *tail; // Last element in the list.
	pthread_mutex_t mutex; // Mutex for locking functions. Makes it thread-safe.
};

struct threadSafeList *listCreate(void);										// Create list
void listDestroy(struct threadSafeList *l);									// Destroy list
struct listItem *listAdd(struct threadSafeList *l, void *content);			// Add new item with contents of a pointer ptr
int listRemove(struct threadSafeList *l, void *content);					// Remove item with contents of a pointer ptr
void listJoinThreads(struct threadSafeList *l);								// Join threads
void * listGetMemoryLocation(struct threadSafeList *l, pthread_t thread_id);// Get meta data about the small heap linked to the POSIX thread with ID pthread_t thread_id.
void * listGetMallocedMemoryAdr(struct threadSafeList *l, void * adr); 		// Get address of memory allocated with malloc(). Used when it needs t be freed with free().
#endif
