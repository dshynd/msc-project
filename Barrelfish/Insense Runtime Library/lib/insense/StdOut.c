#include <insense/StdOut.h>
#include <insense/IString.h>
#include <insense/DAL_mem.h>
#include <insense/cstring.h>
#include <string.h>
#include <stdio.h>
#include <math.h>


// Global Print Any function (can be accessed directly from Insense as proc)

void printAny_proc(void *this, void *handler, AnyTypePNTR param ){
  // Write output
  if(param != NULL){
    AnyTypePNTR data = NULL;
    DAL_assign(&data, param);
    float f = 0.0;
    if(strcmp(data->type, "i")==0)
      printf("%i", data->value.int_value);
    else if(strcmp(data->type, "u")==0)
      printf("%u", data->value.unsigned_value);
    else if(strcmp(data->type, "s")==0)
      printf("%s", ((StringPNTR) data->value.pointer_value)->data);
    else if( strcmp(data->type, "r")==0){
      f = data->value.real_value;
      if(((int)f)==0 && f < 0)
	printf("-%i.%02i", (int) f, (int) (fabs(f - (int) f) * 100) );
      else
	printf("%i.%02i", (int) f, (int) (fabs(f - (int) f) * 100) );
    }
    else if(strcmp(data->type, "b")==0){
      if(data->value.bool_value==true)
	printf("true");
      else
	printf("false");
    }
    else if(strcmp(data->type, "8")==0)
      printf("%u", data->value.byte_value);
    else
      printf("\n?? how to print type %s\n", data->type);
    
    DAL_assign(&data, NULL); // notify garbage collector finished with data
  }
  else
    printf("\n?? can't print NULL ;-( ??\n");  
}
