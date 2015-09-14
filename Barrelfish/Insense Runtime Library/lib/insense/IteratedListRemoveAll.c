
#include <insense/IteratedList.h>      // for linked list
#include <insense/DAL_error.h>         // for DAL_error macro
static char *file_name = "IteratedList";
// removes all elements from a list l
// by assigning null to each node payload the ref count for 
// the payload is decremented

void removeAllElements(List_PNTR l){
  if(l==NULL){
    DAL_error(NULL_POINTER_ERROR);
    return;
  }
  if(isEmpty(l)){
#if DEBUG
    DAL_error(EMPTY_CONTAINER_ERROR);
#endif
    return;    
  }
  ListNode_PNTR previous, current = l->first;
  while( current != NULL){
    previous = current;
    current = current->tail;
    listFreeNode(previous); // free memory used by list node
  }
  l->first = l->next = NULL;
}
