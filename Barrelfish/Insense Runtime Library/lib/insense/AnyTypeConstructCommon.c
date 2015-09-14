#include <insense/AnyType.h>
#include <insense/DAL_mem.h>
#include <insense/DAL_error.h>
#include <insense/cstring.h>
#include <string.h>

#ifndef DALSMALL
static char *file_name = "AnyType"; // used by DAL_error macro
#endif
// private function pre-decls
static void AnyType_decRef(AnyTypePNTR this);

// public functions

AnyTypePNTR Construct_AnyType(void){
   //   printf("Construct_AnyType\n");
  AnyTypePNTR this = (AnyTypePNTR) DAL_alloc(sizeof(AnyTypeStruct), true);
  if( this == NULL ) {
    DAL_error(OUT_OF_MEMORY_ERROR);
    return NULL;
  }
  this->decRef = AnyType_decRef;
  return this;
}


// private function to decerement the ref for Any types for garbage collection
static void AnyType_decRef(AnyTypePNTR this){
  if(strcmp(this->type, "s")==0 ||
     stringStartsWith(this->type, "S") ||
     stringStartsWith(this->type, "A") ||
     stringStartsWith(this->type, "I")||
     stringStartsWith(this->type, "O"))
    DAL_decRef(this->value.pointer_value);
}
