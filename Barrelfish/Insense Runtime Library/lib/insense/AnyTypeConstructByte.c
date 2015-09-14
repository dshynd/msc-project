#include <insense/AnyType.h>
#include <stdint.h>

#ifndef NULL
#define NULL   ((void *) 0)
#endif

AnyTypePNTR Construct_ByteAnyType0(uint8_t value, char* type) {
  AnyTypePNTR this = Construct_AnyType();
  if(this != NULL){
    this->value.byte_value = value;
    this->type = type;
  }
  return this;
}
