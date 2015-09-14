/* 
 * events.c
 *
 * Functions used to support exception handling 
 *
 * @author jonl
 *
 */

#include <insense/events.h>

// returns true when the event is a catchable exception
bool is_exception_event(inceos_event_t event){
  return event >= GENERAL_EXCEPTION_EVENT;
}
