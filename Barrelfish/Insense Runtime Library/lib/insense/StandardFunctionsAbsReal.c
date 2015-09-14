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


float absReal_proc(void *this, void *handler, float r) {
  return (r<0.0?-r:r);
} 
