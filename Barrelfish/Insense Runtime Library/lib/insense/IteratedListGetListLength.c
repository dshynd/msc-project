
#include <insense/IteratedList.h>      // for linked list
#include <insense/DAL_error.h>

static char *file_name = "IteratedList";
unsigned getListLength(List_PNTR l){
  unsigned count = 0;
  if(l==NULL){
    DAL_error(NULL_POINTER_ERROR);
    unsigned int pntr = (uintptr_t) NULL;	
    return(pntr);
  }
  ListNode_PNTR current = l->first;
  while (current != NULL){
    current = current->tail;
    count++;
  }
  return(count);
}
