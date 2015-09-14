/** 
 *
 * String defs for error reporting
 *
 * @author jonl
 *
 */

#include <insense/DAL_error.h>


// Error message strings

char *DAL_error_strings[]={
  "NULL pointer", "NULL element", "Duplicate element", 
  "Empty container", "Element not found", "Index out of bounds",
  "Not connected", "No default arm & empty guard ready",
  "Out of memory", "Serialisation", "Memory still referenced",
  "Unknown sensor type"
};

#ifndef DALSMALL

void DAL_print_error(char* file_name, int line_no, int error_no){
  PRINTF("\nError in file %s on line %i: %s\n",
	 file_name, line_no, DAL_error_strings[error_no]);
}

#endif


