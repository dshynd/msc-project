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
#include <stdint.h>
#include <insense/print_tc.h>

#define MAX_BYTE_STRING_LENGTH 8

// Global printTCByte function (can be accessed directly from Insense as proc)

void printTCByte_proc(void *this, void *handler, uint8_t param ){
	char byte_string[MAX_BYTE_STRING_LENGTH];
	sprintf(byte_string, "%u", param);
	print_tc(byte_string);
}
