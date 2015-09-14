#include <insense/AnyType.h>

// public functions

bool anyTypeGetBoolValue( AnyTypePNTR this ) {
  return this->value.bool_value;
}
