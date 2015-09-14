#include <insense/AnyType.h>

// public functions

void* anyTypeGetPointerValue( AnyTypePNTR this ) {
  return this->value.pointer_value;
}
