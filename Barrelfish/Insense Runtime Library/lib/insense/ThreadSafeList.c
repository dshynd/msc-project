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

#include <stdlib.h>
#include <stdio.h>
#include <insense/GlobalVars.h>
#include <insense/ThreadSafeList.h>
#include <pthread.h>
#include <insense/component.h>

#if MCDEBUG
#include <stdio.h>
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


struct threadSafeList* threadList;
struct threadSafeList* SHList;

/*
 * Create a new list.
 */
struct threadSafeList * listCreate(void) {
	struct threadSafeList *list = (struct threadSafeList *) malloc(sizeof(struct threadSafeList));
	list->count = 0;
	list->head = NULL;
	list->tail = NULL;
	// Lock mutex.
	pthread_mutex_init(&(list->mutex), NULL );
	return list;
}

/*
 *  Destroy the list.
 */
void listDestroy(struct threadSafeList *l) {
	struct listItem *listItem, *tempListItem;
	// Lock mutex.
	pthread_mutex_lock(&(l->mutex));
	// Destroy the item.
	if (l != NULL ) {
		listItem = l->head;
		while (listItem != NULL ) {
			tempListItem = listItem->next;
			free(listItem);
			listItem = tempListItem;
		}
	}
	// Unlock mutex.
	pthread_mutex_unlock(&(l->mutex));
	pthread_mutex_destroy(&(l->mutex));
	// Free memory.
	free(l);
}

/*
 *  Add a new item.
 */
struct listItem * listAdd(struct threadSafeList *l, void *content) {
	struct listItem *listItem;
	// Lock mutex.
	pthread_mutex_lock(&(l->mutex));
	// Allocate some memory.
	listItem = (struct listItem *) malloc(sizeof(struct listItem));
	listItem->value = content;
	listItem->next = NULL;
	listItem->prev = l->tail;
	// Handle tail and head.
	if (l->tail == NULL ) {
		l->head = l->tail = listItem;
	} else {
		l->tail->next = listItem;
		l->tail = listItem;
	}
	l->count++; // Increase count of threads in the list.
	// Unlock mutex.
	pthread_mutex_unlock(&(l->mutex));
	return listItem;
}

/*
 *  Remove an item.
 */
int listRemove(struct threadSafeList *l, void *content) {
	int result = 0;
	struct listItem *listItem = l->head;
	// Lock mutex.
	pthread_mutex_lock(&(l->mutex));

	while (listItem != NULL ) {
		if (listItem->value == content) {
			if (listItem->prev == NULL ) {
				l->head = listItem->next;
			} else {
				listItem->prev->next = listItem->next;
			}

			if (listItem->next == NULL ) {
				l->tail = listItem->prev;
			} else {
				listItem->next->prev = listItem->prev;
			}
			l->count--;
			free(listItem);
			result = 1;
			break;
		}
		listItem = listItem->next;
	}
	// Unlock mutex.
	pthread_mutex_unlock(&(l->mutex));
	return result; // 1 if success, 0 if failure.
}

/*
 *  Join threads inside of the list.
 */
void listJoinThreads(struct threadSafeList *l) {
	struct listItem *listItem;
	void *status; // Status code received from the pthread_join.
	// Loop through thread in the list and call pthread_join on them.
	listItem = l->head;
	while (listItem != NULL ) {
		pthread_join((pthread_t) listItem->value, &status);
		listItem = listItem->next;
	}
}

/*
 * Get meta data about the small heap linked to the POSIX thread with ID pthread_t thread_id.
 */
void * listGetMemoryLocation(struct threadSafeList *l, pthread_t thread_id) {
	// Loop through elements in the list and find one that has *t in it.
	struct listItem *listItem;
	// Loop through thread in the list and call pthread_join on them.
	listItem = l->head;
	while (listItem != NULL ) {
		struct shMapType * shMapItem = (struct shMapType *) listItem->value;

		if ( shMapItem->thread_id == thread_id) {
			return shMapItem;
		}
		listItem = listItem->next;
	}
	return NULL;
}

/*
 * Get address of memory allocated with malloc(). Used when it needs t be freed with free().
 */
void * listGetMallocedMemoryAdr(struct threadSafeList *l, void * adr) {
	// Loop through elements in the list and find one that has *t in it.
	struct listItem *listItem;
	// Loop through threads in the list and call pthread_join on them.
	listItem = l->head;
	while (listItem != NULL ) {
		if (adr == listItem->value) {
			return adr;
		}
		listItem = listItem->next;
	}
	return NULL;
}
