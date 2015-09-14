
#include <insense/IteratedList.h>      // for linked list
#include <insense/DAL_mem.h>           // for memory management
#include <insense/DAL_error.h>         // for DAL_error macro
static char *file_name = "IteratedList";

static ListNode_PNTR Construct_ListNode(void){
  // Not maintaining list nodes using garbage collector, waste of space and 
  // time as you always know when a node has been created and removed from the
  // list, so using BASE_mem_alloc here and will use explicit BASE_mem_free
  // in listFreeNode function (declared above & defined at bottom of file)
  //ListNode_PNTR this=(ListNode_PNTR) DAL_alloc(sizeof(ListNodeStruct),false);
  ListNode_PNTR this =(ListNode_PNTR) BASE_mem_alloc(sizeof(ListNodeStruct));
  if(this == 0){
    DAL_error(OUT_OF_MEMORY_ERROR);
    return 0;
  }
  // need set to NULL here as using BASE_mem_alloc which does not set mem to 0
  this->payload = NULL;
  this->tail = NULL;
  return this;
}


void insertElement(List_PNTR l, void *element) {
  bool emptyList;
  emptyList = isEmpty(l);
  if(l==NULL){
    DAL_error(NULL_POINTER_ERROR);
    return;
  }
  // putting NULL in the list is an issue if the list is supposed
  // to contain valid references to objects, so prevented here 
  if(element == NULL){
    DAL_error(NULL_ELEMENT_ERROR);
    return;
  }
  // duplicates in list are also prevented here during debug
#if DEBUG
  if(!emptyList && containsElement(l, element)){
    DAL_error(DUPLICATE_ELEMENT_ERROR);
    return;
  }
#endif
  ListNode_PNTR newNode = Construct_ListNode();
  if(newNode == NULL){
    DAL_error(OUT_OF_MEMORY_ERROR);
    return;
  }
  DAL_assign(&newNode->payload, element); // increases ref count on element
  //newNode->payload = element; // or if payload not garbage collected
  newNode->tail = l->first;
  l->first = newNode;
  if(emptyList){ // we are about to add the first element to this list set the 
    l->next = l->first; // iterator index to point to the first node
  }
} 
