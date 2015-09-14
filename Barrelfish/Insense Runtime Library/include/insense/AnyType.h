#ifndef ANYTYPE_H_
#define ANYTYPE_H_

#include <insense/Bool.h>
#include <stdint.h>

// The union definition for anys
typedef union any_type {
  int int_value;
  unsigned unsigned_value;
  float real_value;
  bool bool_value;
  uint8_t byte_value;
  void* pointer_value;
} AnyTypeUNION;

// The object definition for anys
typedef struct AnyType_data *AnyTypePNTR, AnyTypeStruct;
struct AnyType_data {
  void (*decRef)(AnyTypePNTR pntr);
  struct AnyType_funcs *impl;
  char* type;
  AnyTypeUNION value;
};

extern AnyTypePNTR Construct_IntAnyType0(int value, char* type);
extern AnyTypePNTR Construct_UnsignedIntAnyType0(unsigned value, char* type);
extern AnyTypePNTR Construct_RealAnyType0(float value, char* type);
extern AnyTypePNTR Construct_BoolAnyType0(bool value, char* type);
extern AnyTypePNTR Construct_ByteAnyType0(uint8_t value, char* type);
extern AnyTypePNTR Construct_PointerAnyType0(void* value, char* type);
extern AnyTypePNTR Construct_AnyType(void);


extern bool anyTypeIsEqual(AnyTypePNTR this, char* compare);
extern char* anyTypeGetType(AnyTypePNTR this);
extern int anyTypeGetIntValue(AnyTypePNTR this);
extern unsigned anyTypeGetUnsignedIntValue(AnyTypePNTR this);
extern float anyTypeGetRealValue(AnyTypePNTR this);
extern bool anyTypeGetBoolValue(AnyTypePNTR this);
extern unsigned char anyTypeGetByteValue(AnyTypePNTR this);
extern void* anyTypeGetPointerValue(AnyTypePNTR this);

#endif /*ANYTYPE_H_ */

