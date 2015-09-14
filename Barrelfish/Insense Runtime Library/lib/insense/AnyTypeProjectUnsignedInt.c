#include <insense/AnyType.h>

// public functions

unsigned anyTypeGetUnsignedIntValue( AnyTypePNTR this ) {
  return this->value.unsigned_value;
}
