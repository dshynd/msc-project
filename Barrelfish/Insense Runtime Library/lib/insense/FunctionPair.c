/**
 * FunctionPair.c
 * Impl file for struct containing references to a function pair. 
 * Will be used to hold references to serialiser and deserialiser functions.
 *
 * @author jonl
 *
*/

#include <insense/FunctionPair.h>
#include <insense/DAL_mem.h>
#include <insense/DAL_error.h>
#include <insense/Bool.h>

#ifndef DALSMALL
static char *file_name = "FunctionPair"; // used by DAL_error macro
#endif

FunctionPairPNTR Construct_FunctionPair(serialf_t ser, deserialf_t des){
//    printf("Construct_FunctionPair\n");
  FunctionPairPNTR this;
  if(ser == NULL && des == NULL){
    DAL_error(NULL_ELEMENT_ERROR);
  } 
  this = DAL_alloc(sizeof(FunctionPairStruct), false);
  if(this == NULL){
    DAL_error(OUT_OF_MEMORY_ERROR);
    return NULL;
  }
  this->ser = ser;
  this->des = des;
  return this;
} 

