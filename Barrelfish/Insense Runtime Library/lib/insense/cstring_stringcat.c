/*
 * cstring.c 
 *
 * to provide some common ops on strings in c
 *
 */

#include <insense/cstring.h>
#include <insense/DAL_mem.h>

char *stringcat(char *s1, char *s2){
	int i=0, c=0, len1 = strlen(s1), len2 = strlen(s2);
	char *result = DAL_alloc((len1 + len2 + 1), false);
	for(i=0; i<len1; i++){
		result[c++] = s1[i];
	}
	for(i=0; i<len2; i++){
		result[c++] = s2[i];
	}
	result[c] = '\0';
	return result;
}
