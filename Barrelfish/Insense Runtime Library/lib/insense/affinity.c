/*
 * Setting affinity to POSIX threads.
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

#include <insense/affinity.h>
#include <insense/GlobalVars.h>

#if MCDEBUG
#define PRINTFMC(...) printf(__VA_ARGS__)
#else
#define PRINTFMC(...)
#endif

#ifndef NULL
#define NULL   ((void *) 0)
#endif

int lastUsedCore = -1; // Note down ID of the core that was used for setting affinity last time.

/*
 * Retrieve a number of cores available on the machine
 * Platform and system independent.
 */
int receiveNumberCores(void) {
	return sysconf(_SC_NPROCESSORS_ONLN);
}

/*
 * Set affinity of POSIX thred with pthread_t thread.
 * AFFINITY_ALGO is defined in GlobalVars.h. Refer to that file for possible values.
 */
int setAffinity(pthread_t thread) {
	return 1;
}

/*
 * Set affinity of POSIX thread "thread" to core with ID defined by "core".
 *
 * Current version is compatible only with Linux. Mac OS requires a different set of tools for setting affinity.
 */
int setAffinityToCore(pthread_t thread, int core) {
	return 0;
}

/*
 * Check affinity of a POSIX thread with pthread_t ID thread
 */
cpu_set_t getAffinityThread(pthread_t thread) {
	return 0;
}

/*
 * Receive next core to be used for affinity.
 * Deprecated in the current implementation.
 */
int receiveCoreIdForThread(pthread_t thread) {
	return -1;
}
