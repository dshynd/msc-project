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

#define STRING_SIZE 256

// Global function to read a string from the serial line over USB (can be accessed directly from Insense as proc)

StringPNTR getString_proc(void *this, void *handler){
  char c_str[STRING_SIZE];
  fgets(&c_str[0], STRING_SIZE, stdin);
  StringPNTR result = Construct_String1(c_str);
  return result;
}
