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


// A function to parse an integer from an Insense string

unsigned parseUnsignedInt_proc(void *this, void *handler, StringPNTR s){
	return (unsigned)parseInt_proc(this, handler, s);
}
