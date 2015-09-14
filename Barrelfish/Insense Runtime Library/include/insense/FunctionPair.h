/**
 * FunctionPair.h
 * Header file for struct containing references to a function pair. 
 * Will be used to hold references to serialiser and deserialiser functions.
 *
 * @author jonl
 *
*/

#ifndef FUNCTION_PAIR_H_
#define FUNCTION_PAIR_H_

typedef void *(*serialf_t)(void *pntr, int *size);
typedef void *(*deserialf_t)(void *pntr);

typedef struct function_pair FunctionPairStruct, *FunctionPairPNTR;
struct function_pair{
  serialf_t ser;
  deserialf_t des;
};

extern FunctionPairPNTR Construct_FunctionPair(serialf_t ser, deserialf_t des);

#endif
