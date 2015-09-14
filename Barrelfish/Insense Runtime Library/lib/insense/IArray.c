
/* ****************************************************
 *
 *
 *  Simple general purpose array code
 *
 *
 * ****************************************************/

#include <insense/IArray.h>
#include <insense/cstring.h>
#include <insense/DAL_mem.h>
#include <insense/DAL_error.h>
#include <insense/GlobalVars.h>

#ifndef DALSMALL
static char *file_name = "IArray"; // used by DAL_error macro
#endif

// private sucvcess variable for use with array_loc within this module
//static bool success;

// private decRef function
static void IArray_decRef(IArrayPNTR pntr);


// construct an array of given length and type_size
IArrayPNTR Construct_Array(unsigned length, unsigned type_size, void *init,
			   bool contains_pointers ) {
  IArrayPNTR this = (IArrayPNTR) DAL_alloc(sizeof(IArrayStruct), true);
  if( this == NULL ) {
    DAL_error(OUT_OF_MEMORY_ERROR);
    return NULL;
  }
  this->type_size=type_size; this->decRef=IArray_decRef; 
  this->length=length; // set this->length (will be set to 0 if length is 0)
  if(length == 0) // if length is 0 ensure creation of element 0 to avoid deref
	  length = 1; // problems, but maintain this->length = 0  
  DAL_assign(&this->data, DAL_alloc(type_size * length, contains_pointers));
  if(this->data == NULL){
	  DAL_error(OUT_OF_MEMORY_ERROR);
	  return NULL;
  }
  if(init != NULL){// initialise every element with init
	  int i; void *pntr = this->data;
	  for(i=0; i<length; i++){
		  if(contains_pointers) memncpy(pntr, &init, type_size);
		  else memncpy(pntr, init, type_size);
		  pntr = (((char *) pntr) + type_size);
	  }
	  // if init was pointer, add length n.o. refs to init's memory header
	  if(contains_pointers) DAL_modRef_by_n(init, length);
  }
  return this;
}

// return pointer to array element a[i] or some defualt location 
// if array index out of bounds
void *array_loc(IArrayPNTR a, unsigned i){
  // create memory for location to return when array index out of bounds
  // this is not safe when e.g. trying to access array of structs
	if (i >= a->length){
		i = 0; // do safe deref of array element 0 which is always created
	}
	return ((void *) (((char *) a->data) + i * a->type_size));
}


// private decRef functions
static void IArray_decRef(IArrayPNTR pntr){
  if(DAL_mem_contains_pointers(pntr->data)){
    unsigned i, length;
    length = pntr->length;
    if(length == 0) // if this is a 0-length array and contains pointers 
    	length = 1; // we know 1 element was creayted for safety, so 
                    // garbage collect it here
    for(i=0; i<length; i++)
      DAL_decRef(((void **) pntr->data)[i]); // array decref code
    //DAL_decRef( array_loc(pntr, i, &success) ); // alternative deref code
  }
  DAL_mem_set_contains_pointers(pntr->data, false);
  DAL_decRef(pntr->data);
}




/* ****************************************************
 * 
 * General purpose array code
 *
 * @author jonl
 *
 * ****************************************************/



#if USE_UNTESTED_GP_CODE

// private functions
static void copy_ArrayData(IArrayPNTR copy, IArrayPNTR src,
			   bool contains_pointers, bool copy_data);
static void Array_decRef(IArrayPNTR pntr);



// function to construct an array
IArrayPNTR Construct_Array(unsigned num_dims, unsigned type_size, void *init, 
			   bool contains_pointers, int *lengths){
  //if(*lengths <= 0) return NULL; // have reached end of lengths array
  IArrayPNTR this = (IArrayPNTR) DAL_alloc(sizeof(IArrayStruct), true);
  if( this == NULL ) {
    DAL_error(OUT_OF_MEMORY_ERROR);
    return NULL;
  }
  this->decRef = Array_decRef; this->num_dims = num_dims;
  this->copyObject = copy_Array; this->length = lengths[0]; 
  this->type_size = (num_dims > 1 ? sizeof(IArrayPNTR) : type_size);
  bool elements_are_pointers = (num_dims>1 ? true : contains_pointers);
  DAL_assign(&this->data, (IArrayPNTR*) 
	     (DAL_alloc(this->length * this->type_size, elements_are_pointers)));
  if(num_dims > 1){ // need array of arrays
    int i;
    for(i=0; i<this->length; i++){
      DAL_assign( &((IArrayPNTR *) this->data)[i], 
		  Construct_Array(num_dims-1, type_size, init, contains_pointers, 
				  lengths+1));
    }
    return this;
  }
  // if we get to here num_dims == 1 (we have got to last dim)
  // should fill array with init if it is not set to NULL
  if(init != NULL){
    int i; void *pntr = this->data;
    for(i=0; i<this->length; i++){
      memncpy(pntr, init, type_size);
      pntr = (((char *) pntr) + type_size);
    }
  }
  return this;
}

// return pointer to array element a[i][j]... where [i][j]... are in the nd_index list
void *array_loc(IArrayPNTR a, bool *success, int *nd_index){
  if(nd_index[0] >= a->length ){
    *success = false;
    return NULL;
  }
  *success = true;
  if(nd_index[1] < 0) // look ahead on index list, have we reached end
    return ((void *) (((char *) a->data) + nd_index[0]*a->type_size));
  else
    return Array_loc(a, success, nd_index+1);
}




// copy the given array and optionally copy the data content if the data are
// of pointer type
IArrayPNTR copy_Array(IArrayPNTR a, bool copy_data){
  unsigned i, type_size;
  bool contains_pointers;
  int lengths[a->num_dims+1];
  IArrayPNTR pntr = a;
  // iterate over dims looking at 0th element
  for(i=0; i<a->num_dims; i++){
    lengths[i] = pntr->length; // record lengths of each dimension
    type_size = pntr->type_size; // get size of data
    pntr = (IArrayPNTR) pntr->data;
  }
  contains_pointers = DAL_mem_contains_pointers(pntr);
  lengths[a->num_dims] = -1; // terminate list of lengths
  IArrayPNTR copy = Construct_Array(a->num_dims, type_size, NULL, 
				    contains_pointers, lengths);
  copy_ArrayData(copy, a, contains_pointers, copy_data);
  return copy;
}


// Old 1-D copy_Array function only left here for reference
IArrayPNTR copy_Array_old(IArrayPNTR this){
  bool contains_pointers = DAL_mem_contains_pointers(this->data);
  IArrayPNTR copy = 
    Construct_Array(this->length, this->type_size, NULL, contains_pointers);
  if(copy == NULL) 
    return NULL;
  int i;
  if(contains_pointers)
    for(i=0; i < this->length; i++)
      DAL_assign( &(((void**)copy->data)[i]) , ((void **)this->data)[i] );
  else
    memncpy(copy->data, this->data,  this->type_size * this->length);
  return copy;
}





// private utility functions

// function to copy the array data
static void copy_ArrayData(IArrayPNTR copy, IArrayPNTR src,
			   bool contains_pointers, bool copy_data){
  unsigned i;
  if(src->num_dims > 1) { // must get down to data
    for(i=0; i<src->num_dims; i++)
      copy_ArrayData(copy->data, src->data, contains_pointers, copy_data);
  }
  for(i=0; i<src->length; i++){
    if(contains_pointers){
      if(copy_data){ // must use correct function to copy data
	CopyableObjectPNTR o = ((CopyableObjectPNTR)(((void **)src->data)[i]));
	DAL_assign( &(((void**)copy->data)[i]), o->copyObject(o, false));
      }
      else // no need to copy data, simply put src pointers into copy array
	DAL_assign( &(((void**)copy->data)[i]), 
		    ((void **)src->data)[i] );
    }
    else // contains simple types, so copy simple types over
      memncpy(copy->data, src->data,  src->type_size * src->length);
  }
  return;
}


// private decRef function
static void Array_decRef(IArrayPNTR pntr){
  if(DAL_mem_contains_pointers(pntr->data)){
    unsigned i;
    for(i=0; i<pntr->length; i++)
      DAL_decRef(((void **) pntr->data)[i]);
  }
  DAL_decRef(pntr->data);  
}

#endif

