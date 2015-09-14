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

#include <insense/DAL_random.h>

// returns a pseudo random number between 0 and max
unsigned short DAL_random(unsigned short max){
  return (rand() % (max+1));
}
