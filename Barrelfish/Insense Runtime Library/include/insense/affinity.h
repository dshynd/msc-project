/*
 * Setting affinity to POSIX threads.
 *
 *
 * Copyright (c) 2013, Pavlo Bazilinskyy <pavlo.bazilinskyy@gmail.com>
 * modified by Jon Lewis 2013 after submissions
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

#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>

// Added by jonl

#define AFFINITY_DYNAMIC      0
#define AFFINITY_ROUND_ROBIN  1
#define AFFINITY_SINGLE_CORE  2

typedef int cpu_set_t;

// end added by jonl

//#include <sys/syscall.h>
#include <errno.h>

#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

extern int lastUsedCore;

int setAffinity(pthread_t thread); 					// Set affinity of POSIX thred with pthread_t thread.
int setAffinityToCore(pthread_t thread, int core); 	// Set affinity of POSIX thread "thread" to core with ID defined by "core".
int receiveNumberCores(void);							// Retrieve a number of cores available on the machine.
cpu_set_t getAffinityThread(pthread_t thread);		// Check affinity of a POSIX thread with pthread_t ID thread.
int receiveCoreIdForThread(pthread_t thread);		// Receive next core to be used for affinity.
