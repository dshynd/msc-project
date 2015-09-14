
#include <insense/IteratedList.h>      // for linked list
#include <insense/DAL_mem.h>           // for memory management

// Explicit free for list node structures .
// As list nodes cannot be shared among different lists at present, memory 
// for nodes is maintained manually and not garbage collected.
// In contrast, memory for list payload is garbage collected, may have same
// item in many lists ... so listFreeNode calls DAL_decRef on these objects

void listFreeNode(ListNode_PNTR node){
  // force ref count decrement on object being kept in node payload
  DAL_decRef(node->payload);
  BASE_mem_free(node);
}

