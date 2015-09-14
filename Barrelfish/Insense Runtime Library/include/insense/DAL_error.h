/** 
 *
 * Simple utilities for error reporting
 *
 * @author jonl
 *
 */
   
#ifndef DAL_ERROR_H_
#define DAL_ERROR_H_

#include<stdio.h>

#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


typedef enum { 
  NULL_POINTER_ERROR=0, NULL_ELEMENT_ERROR, DUPLICATE_ELEMENT_ERROR, 
  EMPTY_CONTAINER_ERROR, ELEMENT_NOT_FOUND_ERROR, INDEX_OUT_OF_BOUNDS_ERROR,
  NOT_CONNECTED_ERROR, SELECT_NO_RESULT_ERROR,
  OUT_OF_MEMORY_ERROR, SERIALISATION_ERROR, STILL_REFERENCED_ERROR,
  UNKNOWN_SENSOR_ERROR, CHAN_OUT_OF_MEMORY_ERROR
} DAL_errno_t;

extern char *DAL_error_strings[];

#ifndef DALSMALL

//void DAL_print_error(char *file_name, int line_no, DAL_errno_t errno);
void DAL_print_error(char* file_name, int line_no, int error_no);
#define DAL_error( errno ) DAL_print_error(file_name, __LINE__ , errno)
//#define DAL_error( errno ) DAL_print_error(errno)

#else

#define DAL_error( errno ) PRINTF(DAL_error_strings[errno]);

#endif  // DALSMALL

#endif // DAL_ERROR_H_
