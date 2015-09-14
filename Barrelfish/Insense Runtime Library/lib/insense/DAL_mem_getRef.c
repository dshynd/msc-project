/*
 * Memory management functions
 *
 * @author jonl
 *
 */

#include <insense/DAL_mem.h>
#include <insense/DAL_error.h>

#ifndef NULL
#define NULL   ((void *) 0)
#endif

#ifndef DALSMALL
static char *file_name = "DAL_mem_getRef"; // used by DAL_error macro
#endif


/*
 * returns the reference count for a memory segment allocated by DAL_alloc
 */
unsigned DAL_getRef(void *pntr){
  if(pntr==NULL){
    DAL_error(NULL_POINTER_ERROR);
    return 0;
  }
  MemHeader header = (MemHeader) (pntr - sizeof(MemHeaderStruct));
  return(header->ref_count);
}
