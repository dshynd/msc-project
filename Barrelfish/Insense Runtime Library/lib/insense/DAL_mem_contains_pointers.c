/*
 * Memory management functions
 *
 * @author jonl
 *
 */

#include <insense/DAL_mem.h>

#ifndef DALSMALL
//static char *file_name = "DAL_mem_contains_pointers"; // used by DAL_error macro
#endif


bool DAL_mem_contains_pointers(void *pntr){
  return ( ( (MemHeader) (( (char *)pntr ) 
			  - sizeof(MemHeaderStruct)))->mem_contains_pointers );
}


void DAL_mem_set_contains_pointers(void *pntr, bool mem_contains_pointers){
  ( (MemHeader) (( (char *)pntr ) 
			  - sizeof(MemHeaderStruct)))->mem_contains_pointers = 
    mem_contains_pointers;
}
