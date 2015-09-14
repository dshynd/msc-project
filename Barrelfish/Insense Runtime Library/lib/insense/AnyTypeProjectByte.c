#include <insense/AnyType.h>
#include <stdint.h>

// public functions

uint8_t anyTypeGetByteValue( AnyTypePNTR this ) {
  return this->value.byte_value;
}
