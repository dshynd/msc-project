
#include <insense/IteratedList.h>      // for linked list
#include <stdio.h>

// Returns whether the list is empty
bool isEmpty(List_PNTR l){
  if(l==NULL)
    return true;
  return(l->first == NULL);
}
