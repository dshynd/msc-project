/*
 * Standard functions header file
 * 
 * Declares standard functions for use in Insense
 *
 * @author jonl
 *
 */

#ifndef STANDARD_FUNCTIONS_H_
#define STANDARD_FUNCTIONS_H_

#include <insense/IArray.h>
#include <insense/Bool.h>
#include <insense/AnyType.h>
#include <insense/IString.h>
#include <stdint.h>

// dummy sensor function
int tempReading_proc(void *this, void *op_status);


// General purpose utility functions
uint8_t intToByte_proc(void *this, void *op_status, int i);
float intToReal_proc(void *this, void *handler, int i);
unsigned intToUnsigned_proc(void *this, void *op_status, int i); 
bool byteArrayEqual_proc(void *this, void *op_status, IArrayPNTR a, IArrayPNTR b);
float avgIntArray_proc(void *this, void *op_status, IArrayPNTR iarray);
float avgRealArray_proc(void *this, void *op_status, IArrayPNTR rarray);
int absInt_proc(void *this, void *op_status, int i);
float absReal_proc(void *this, void *op_status, float r);
int squareInt_proc(void *this, void *op_status, int i);
float squareReal_proc(void *this, void *op_status, float r);
float squareRootInt_proc(void *this, void *op_status, int i);
float squareRoot_proc(void *this, void *op_status, float r);
int realToInt_proc(void *this, void *op_status, float r);
void printString_proc(void *this, void *op_status, StringPNTR s);
void printInt_proc(void *this, void *op_status, int i);
void printReal_proc(void *this, void *op_status, float f);
void printUnsignedInt_proc(void *this, void *op_status, unsigned u);
void printByte_proc(void *this, void *op_status, uint8_t b);
void printTCString_proc(void *this, void *handler, StringPNTR param );
void printTCInt_proc(void *this, void *handler, int param );
void printTCReal_proc(void *this, void *handler, float f );
void printTCUnsignedInt_proc(void *this, void *handler, unsigned param );
void printTCByte_proc(void *this, void *handler, uint8_t param );

StringPNTR getString_proc(void *this, void *handler);
bool stringEquals_proc(void *this, void *handler, StringPNTR param1, StringPNTR param2);
unsigned parseUnsignedInt_proc(void *this, void *handler, StringPNTR s);
int parseInt_proc(void *this, void *handler, StringPNTR s);
float parseReal_proc(void *this, void *handler, StringPNTR s);

void setCore_proc(void *this, void *handler, unsigned core);

float randReal_proc(void *this, void *handler);

#endif /* STANDARD_FUNCTIONS_H_ */
