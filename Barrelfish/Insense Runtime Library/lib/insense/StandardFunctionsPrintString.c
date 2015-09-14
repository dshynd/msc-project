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
#include <stdio.h>
#include <insense/DAL_mem.h>

// Global Print Any function (can be accessed directly from Insense as proc)

void printString_proc(void *this, void *handler, StringPNTR str ){
	// Write output
	if(str != NULL){
		DAL_incRef(str);
		printf("%s", str->data);
		DAL_decRef(str);
	}
}
