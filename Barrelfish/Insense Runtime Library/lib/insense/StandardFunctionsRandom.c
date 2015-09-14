/*
 * Standard functions impl file
 * 
 * Defines dummy temp reading function
 *
 * @author jonl
 *
 */

#include <insense/StandardFunctions.h>
#include <insense/events.h>
#include <stdlib.h>

float randReal_proc(void *this, void *handler){
	return ((float) rand() / (float) RAND_MAX);
}

