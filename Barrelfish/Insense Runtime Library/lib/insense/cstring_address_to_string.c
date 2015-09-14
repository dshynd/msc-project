/*
 * cstring.c 
 *
 * to provide some common ops on strings in c
 *
 */

#include <insense/cstring.h>

/*
 * Convert a pointer to a base 10 string representation
 * @param pntr The pointer to be converted
 * @return the character representation of pntr in base 10
 */
char* address_to_string( void *pntr ) {
	unsigned int vpntr = (uintptr_t) pntr;
	return myitoa(vpntr, 10);
}
