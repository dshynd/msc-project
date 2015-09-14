#include <string.h>
#include <insense/AnyType.h>

// public functions

bool anyTypeIsEqual( AnyTypePNTR this, char* compare ) {
  return (strcmp(this->type, compare) == 0);
}
