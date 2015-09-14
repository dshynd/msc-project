/*
 * Standard functions impl file
 * 
 * Defines functions for use in Insense
 *
 * @author jonl
 *
 */

#include <insense/StandardFunctions.h>
#include <insense/DAL_mem.h>
#include <insense/events.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>


// A function to parse an integer from an Insense string

int parseInt_proc(void *this, void *handler, StringPNTR s){
	StringPNTR s_local = NULL;
	DAL_assign(&s_local, s);
	int i = atoi(s_local->data);
	DAL_assign(&s_local, NULL);
	return i;
}
