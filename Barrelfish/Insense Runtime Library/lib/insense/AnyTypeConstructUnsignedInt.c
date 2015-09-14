#include <insense/AnyType.h>

#ifndef NULL
#define NULL   ((void *) 0)
#endif

AnyTypePNTR Construct_UnsignedIntAnyType0(unsigned value, char *type) {
  AnyTypePNTR this = Construct_AnyType();
  if(this != NULL){
    this->value.unsigned_value = value;
    this->type = type;
  }
  return this;
}

