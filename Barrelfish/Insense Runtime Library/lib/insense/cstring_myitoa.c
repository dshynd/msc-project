/*
 * cstring.c 
 *
 * to provide some common ops on strings in c
 *
 */

#include <insense/cstring.h>

/*
 * Convert an integer to string representation in base base
 * Copes with any base up to a maximum of 16.
 * @param val The integer to be converted
 * @param base The base to which the integer should be converted
 * @return the character representation of val in base base
 */
char* myitoa(unsigned int val, int base){
		static char buf[32] = {0};
		int i = 30;
		for(; val && i ; --i, val /= base)
			buf[i] = "0123456789abcdef"[val % base];
		return &buf[i+1];
}
