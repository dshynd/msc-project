/*
 * Memory management functions used with the big heap memory allocation scheme.
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

#ifndef NULL
#define NULL   ((void *) 0)
#endif

pthread_t mainThread;			// ID of the main thread.

/*
 * Allocator of memory, allocates into a big heap. Uses malloc().
 * Allocation is protected by a mutex. Uncoment code to enable extra mutes protection.
 */
void * BH_alloc(unsigned size) {
	// Allocate memory
	void * result = malloc(size);
	// Return
	return result;
}

/*
 * Freeing memory from the big heap. Uses free().
 * Freeing is not protected by a mutex. Uncoment code to enable extra mutes protection.
 */
void BH_free(void *ptr) {
	// Free memory
	free(ptr);
}
