/*
 * IteratedList.c
 * Implementation of a singly linked list of pointers (void *)
 * which allows insertion (at head), removal of specific 
 * nodes idetified by value, and iterated access to elements in the list
 *
 * Coded to contain void* pointers in case we want to use the list for
 * for a variety of objects.
 *
 * As list node structures are not contained in more than one list at present,
 * memory for nodes is maintained manually and not garbage collected.
 * In contrast, memory for the node payload and list object (containing 
 * pointer to first and next elements in iterated list) is garbage collected.
 * The payload of nodes may be any object for which memory is allocated by
 * DAL_alloc. The payload may be contained by many lists, for example, 
 * a single channel may be referenced by many connections lists.
 * The decRef function for the list is called by DAL_decRef (garbage collector)
 * before freeing memory for the list object when the ref count for a list 
 * object reaches zero. The decRef function simply removes all list nodes,
 * explicitly freeing memory used by list nodes after decrementing the ref 
 * count for all elements referenced by the list nodes.
 *
 * @author jonl
 *
 */

#include <insense/IteratedList.h>      // for linked list
#include <insense/DAL_mem.h>           // for memory management
#include <insense/DAL_error.h>         // for DAL_error macro

#define DEBUG 0

#ifndef DALSMALL
static char *file_name = "IteratedList"; // used by DAL_error macro
#endif

// "private" (static in C-sense) function declarations, not included in header.

// for garbage collection
static void List_decRef(List_PNTR pntr);

// End of "private" function declarations


// (public) functions and constructors 
List_PNTR Construct_List(void){
  List_PNTR this = (List_PNTR) DAL_alloc(sizeof(ListStruct), true);
  if(this == 0){
    DAL_error(OUT_OF_MEMORY_ERROR);
    return 0;
  }
  this->decRef = List_decRef;
  // alloc sets to 0
  //this->first = NULL; // initially empty list
  //this->next = NULL;  // iterator pointer has nothing to point to
  return(this);
}


// "private" (static in C-sense) functions, not included in header.


// decRef function is called when ref count to a list object is zero
// before freeing memory for list object
static void List_decRef(List_PNTR pntr){
  if(!isEmpty(pntr))
    removeAllElements(pntr); // forces decRef on list content (node payload)
}

// End of "private" functions




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

