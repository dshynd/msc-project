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
#include <insense/print_tc.h>

#define MAX_UNSIGNED_STRING_LENGTH 8

// Global printTCUnsignedInt function (can be accessed directly from Insense as proc)

void printTCUnsignedInt_proc(void *this, void *handler, unsigned param ){
    char uint_string[MAX_UNSIGNED_STRING_LENGTH];
	sprintf(uint_string, "%u", param);
	print_tc(uint_string);
}
