#include <insense/AnyType.h>

#ifndef NULL
#define NULL   ((void *) 0)
#endif

AnyTypePNTR Construct_RealAnyType0(float value, char* type) {
  AnyTypePNTR this = Construct_AnyType();
  if(this != NULL){
    this->value.real_value = value;
    this->type = type;
  }	
  return this;
}
