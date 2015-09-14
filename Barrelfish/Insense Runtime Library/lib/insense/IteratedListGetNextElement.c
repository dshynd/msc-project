
#include <insense/IteratedList.h>      // for linked list
#include <insense/DAL_error.h>

static char *file_name = "IteratedList";
void* getNextElement(List_PNTR l){
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
  void *element = l->next->payload;
  l->next = l->next->tail;
  if(l->next == NULL)   // if we iterated to the end of the list
    l->next = l->first; // wrap round to the beginning
  return(element);
}
