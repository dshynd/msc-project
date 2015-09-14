#include <insense/AnyType.h>

// public functions

float anyTypeGetRealValue( AnyTypePNTR this ) {
  return this->value.real_value;
}
