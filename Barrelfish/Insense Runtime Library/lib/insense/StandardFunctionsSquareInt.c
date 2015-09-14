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


int squareInt_proc(void *this, void *handler, int i) { 
  return (i*i);
} 
