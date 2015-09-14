
#ifndef IARRAY_H_
#define IARRAY_H_

#include <insense/Bool.h>

/* ****************************************************
 *
 *
 *  Simple general purpose array code
 *
 *
 * ****************************************************/


typedef struct IArray_data *IArrayPNTR, IArrayStruct;
struct IArray_data {
  void (*decRef)(IArrayPNTR pntr);
  unsigned type_size;
  unsigned length;
  void *data;
};

extern IArrayPNTR Construct_Array(unsigned capacity, unsigned type_size, 
				  void *init, bool contains_pointers);
extern void *array_loc(IArrayPNTR a, unsigned index);



#endif /*IARRAY_H_*/

