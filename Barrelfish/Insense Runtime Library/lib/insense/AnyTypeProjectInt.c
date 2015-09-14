#include <insense/AnyType.h>

// public functions

int anyTypeGetIntValue( AnyTypePNTR this ) {
  return this->value.int_value;
}
