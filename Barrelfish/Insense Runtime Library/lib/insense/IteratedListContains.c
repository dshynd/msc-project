
#include <insense/IteratedList.h>      // for linked list
#include <insense/DAL_error.h>         // for DAL_error macro

#if DEBUG
static char *file_name = "IteratedList";
#endif

bool containsElement(List_PNTR l, void *element){
  if(l==NULL){
#if DEBUG
    DAL_error(NULL_POINTER_ERROR);
#endif
    return(false);
  }
  if(isEmpty(l)){
#if DEBUG
    DAL_error(EMPTY_CONTAINER_ERROR);
#endif
    return(false);
  }
  ListNode_PNTR current = l->first;
  while (current != NULL){
    if(current->payload == element)
      return(true);
    current = current->tail;
  }
  return(false);
}
