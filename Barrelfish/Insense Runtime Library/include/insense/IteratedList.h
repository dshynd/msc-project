/**
 * IteratedList.h
 * Header file for singly linked iterated list implementation.
 *
 * @author jonl
 *
*/

#include <insense/Bool.h>
#include <insense/DAL_random.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#ifndef LIST_H_
#define LIST_H_

typedef struct listnode ListNodeStruct, *ListNode_PNTR;
struct listnode {
  void *payload; 
  ListNode_PNTR tail; 
};

typedef struct list ListStruct, *List_PNTR;
struct list {
  void (*decRef)(List_PNTR pntr);
  ListNode_PNTR first;   // the first node in the list, NULL if list is empty
  ListNode_PNTR next;    // used for iterating over lists using getNextElement
};

// constructor
extern List_PNTR Construct_List(void);

// functions to insert into, get and remove elements from a list 
extern void insertElement(List_PNTR l, void *element);

extern void *getNextElement(List_PNTR l);
extern void *getElementN(List_PNTR l, unsigned index);
extern void *getRandomElement(List_PNTR l);

extern unsigned getListLength(List_PNTR l);

extern bool containsElement(List_PNTR l, void *element);
extern bool isEmpty(List_PNTR l);
extern void *searchList(List_PNTR l, int (*f)(void *element, void *key), void *key);

extern void removeElement(List_PNTR l, void *element);
extern void removeAllElements(List_PNTR l);

extern void listFreeNode(ListNode_PNTR node);

// Last function only used for testing, can be commented out to save space
#ifdef DALDEBUG
extern void displayList(List_PNTR l, void (*f)(void *element));
#endif

#endif
