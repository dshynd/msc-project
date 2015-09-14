/*
 * Standard functions impl file
 * 
 * Defines functions for use in Insense
 *
 * @author jonl
 *
 */

#include <insense/StandardFunctions.h>
#include <insense/events.h>


int realToInt_proc(void *this, void *handler, float r) {
  if((r - (int) r) > 0.5)
    return (int) r + 1;
  else
    return (int) r;
} 
