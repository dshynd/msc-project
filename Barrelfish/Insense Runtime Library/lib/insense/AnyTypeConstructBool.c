#include <insense/AnyType.h>

#ifndef NULL
#define NULL   ((void *) 0)
#endif

AnyTypePNTR Construct_BoolAnyType0(bool value, char* type) {
  AnyTypePNTR this = Construct_AnyType();
  if(this != NULL){
    this->value.bool_value = value;
    this->type = type;
  }
  return this;
}
