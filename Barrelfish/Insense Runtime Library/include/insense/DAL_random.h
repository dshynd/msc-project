/*
 * To provide random number generation
 *
 * At present this uses the stdlib rand() function  
 *
 * Can be switched to use contiki pseudo random number generator, 
 * 		see core/lib/rand.[hc] and core/lib/random.[hc]
 * by compiling with CONTIKI_RANDOM flag set
 * @author jonl
 *
 */

#ifndef CONTIKI_RANDOM
#include <stdlib.h>
#else
#include <random.h>
#define rand random_rand
#define srand random_init
#endif

// returns a pseudo random number between 0 and max
unsigned short DAL_random(unsigned short max);
