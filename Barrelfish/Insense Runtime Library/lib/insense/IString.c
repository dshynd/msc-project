/*
 * String "class" (C-module)
 *
 * original author ???
 *
 * Modified by jonl 2008-05-02.
 * Decided to disallow concat on Strings in Insense, so 
 * have removed String functions and altered String objects to contain
 * a char* reference to string literals (that are in .text section of ELF) 
 * and a length field, set at creation using strlen.
 *
 */


//#include <inttypes.h>
#include <string.h>
#include <insense/IString.h>
#include <insense/DAL_mem.h>
#include <insense/DAL_error.h>
#include <insense/cstring.h>

#ifndef DALSMALL
static char *file_name = "String"; // used by DAL_error macro
#endif

// XXX walk-around of implicitly declaring
//extern void * strcmp();

static void String_decRef(StringPNTR p){
	DAL_decRef(p->data);
}




// Create new string object for given char * data. 
// If duplicate is set to true the data memory holding the characters 
// is dynamically allocated and will be garbage collected 
static StringPNTR Construct_string_object( char* data , bool duplicate ){
	StringPNTR this = (StringPNTR) DAL_alloc(sizeof(StringStruct), duplicate);
	if( this == NULL ) {
		DAL_error(OUT_OF_MEMORY_ERROR);
		return (StringPNTR) NULL;
	}
	this->length = strlen(data);
	if( duplicate ){
		char *data_mem = (char*) DAL_alloc(this->length+1, false);
		if(data_mem == NULL){
			DAL_error(OUT_OF_MEMORY_ERROR);
			return (StringPNTR) NULL;
		}
		strcpy(data_mem, data);
		DAL_assign(&this->data, data_mem);
		this->decRef = String_decRef;
	} else {
		this->data = data;
	}
	return this;
}

/*
 * Create String object from char * data parameter
 * The string data is assumed to be in .text section of ELF
 * so no dynamic memory is allocated for string data and
 * the data element of the struct is not garbage collected
 * 
 */
StringPNTR Construct_String0( char* data ) {
  return Construct_string_object(data, false);
}


/*
 * Create String object from char * data parameter
 * Memory for the string data is dynamically allocated 
 * and garbage collected.
 * 
 */
StringPNTR Construct_String1( char* data ) {
  return Construct_string_object(data, true);
}


int stringCompare(StringPNTR param1, StringPNTR param2) {
	// Assign parameters to local vars so as to use garbage collection
	StringPNTR this = NULL, other = NULL;
	int result = -1; // default is that strings differ
	if(param1 != NULL){
		DAL_assign(&this, param1);
	}
	if(param2 != NULL){
		DAL_assign(&other, param2);
	}
	if(this != NULL && other != NULL){
		result = strcmp(this->data, other->data);
	}
	DAL_decRef(this);
	DAL_decRef(other);
	return result;
}

