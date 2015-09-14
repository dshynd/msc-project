
#include <insense/IteratedList.h>      // for linked list
#include <insense/DAL_error.h>         // for DAL_error macro
static char *file_name = "IteratedList";
// Removes an element from the list l, decrements ref count on element
void removeElement(List_PNTR l, void *element) {
  // case: error, l is null, i.e. no list defined
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
  if(element == NULL){
    DAL_error(ELEMENT_NOT_FOUND_ERROR);
    return;
  }
  // case: element we want to remove is right at the start of the list
  if(l->first->payload == element){
    ListNode_PNTR oldFirst = l->first;
    l->first = oldFirst->tail;
    // take action if we are removing the next iterator node
    if(l->next == oldFirst) l->next = l->first; 
    listFreeNode(oldFirst); // explicitly free memory used by node
    return;
  }
  // case: otherwise try to find element to remove
  ListNode_PNTR previous = l->first, current = previous->tail;
  while(current != NULL && current->payload!=element){
    previous = current;
    current = current->tail;
  }
  if(current!=NULL){ // we found the element to remove
    // take action if we are removing the next iterator node
    if(l->next == current) l->next = current->tail;
    if(l->next == NULL) l->next = l->first;
    // deal with removal
    previous->tail = current->tail;
    // explicitly free memory used by node
    listFreeNode(current);
  }
  else
    DAL_error(ELEMENT_NOT_FOUND_ERROR);
}
