/*
 * Defining global variables used in runtime and compiled Insense programs.
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


#ifndef GLOBAL_VARS_H_
#define GLOBAL_VARS_H_

#include <insense/Bool.h>
#include <insense/ThreadSafeList.h>
#include <pthread.h>

#if HEAPS == HEAP_PRIVATE
extern pthread_mutex_t thread_lock; // Mutex used to avoid problems with scheduling that comes when small heaps are utilised.
#endif

#define DEBUG 0			// Output debugging information, defined for InceOS. Possible values: 0, 1, 2.
#define MCDEBUG 0		// Output debugging information, defined for Multi-core Insense. Possible values: 0, 1.
#define TIMING 1		// Measure and output execution time.

#endif
