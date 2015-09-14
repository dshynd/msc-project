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
#include <insense/DAL_mem.h>


// Global Print Any function (can be accessed directly from Insense as proc)

void printTCString_proc(void *this, void *handler, StringPNTR param ){
  // Write output
  if(param != NULL){
    StringPNTR str = NULL;
    DAL_assign(&str, param);
    print_tc(str->data);
    DAL_decRef(str);
  }
}
