
#include <insense/IteratedList.h>      // for linked list
#include <insense/DAL_error.h>

static char *file_name = "IteratedList";
// get hold of Nth element from the list
// elements are indexed from 0 to N-1
void* getElementN(List_PNTR l, unsigned index){
  unsigned count = 0;
  if(l==NULL){
    DAL_error(NULL_POINTER_ERROR);
    return(NULL);
  }
  if(isEmpty(l)){
#if DEBUG
    DAL_error(EMPTY_CONTAINER_ERROR);
#endif
    return(NULL);
  }
  ListNode_PNTR current = l->first;
  while (current != NULL  &&  count < index){
    current = current->tail;
    count++;
  }
  if(current == NULL){
#if DEBUG
    DAL_error(INDEX_OUT_OF_BOUNDS_ERROR);
#endif
    return(NULL);
  }
  return(current->payload);
}
