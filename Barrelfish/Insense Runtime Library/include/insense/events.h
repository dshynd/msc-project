/* 
 * events.h
 *
 * Definitnion of integer values used by the system to indicate 
 * 
 *  - channel success or failure internally to InceOS
 *  - radio (INCH) communication events (yet to be implemeted) 
 *  - success (operation succeeded, the procedure returned inidcating ok, ...) 
 *  - the occurrence of exceptions 
 *
 * GENERAL_SUCCESS_EVENT indicates succeess, e.g. the operation succeeded, the procedure returned normally (no exception thrown)
 * The first exception is at 1+GENERAL_SUCCESS_EVENT and all other exceptions follow.
 *
 * @author jonl
 *
 */


// NOTE: some of these event values were required under Contiki but may not be needed under InceOS 


#ifndef EVENTS_H
#define EVENTS_H

#include <insense/Bool.h>

typedef enum { 
  BAD_CHAN_EVENT = 1,                        // channel index is out of range or its usage flags are not set
  CHAN_RECV_NODATA_EVENT,                    // blocked receiver process woken without data to receive (interrupt woke up rcvr)
  CHAN_RECV_BUFF_EMPTY_EVENT,                // returned by dequeue and send when q is empty
  CHAN_RECV_EVENT,                           // successful receive from channel 
  CHAN_SEND_EVENT,                           // successful send over channel
  RADIO_SEND_EVENT,                          // radio has sent data
  RADIO_RECV_EVENT,                          // radio has received data
  CHAN_CONNECT_EVENT,                        // channel has been connected
  PUBLIC_CHAN_QUERY_EVENT,                   // query remote node about its pub lic channels
  NEIGHBOUR_QUERY_EVENT,                     // query remote node about its neighbours
  GENERAL_SUCCESS_EVENT,                     // general success inidicator
  GENERAL_EXCEPTION_EVENT                    // general exception inidcator
} inceos_event_t;


// Exception events which can be caught in Insense programs
typedef enum {
  // exceptions start at GENERAL_EXCEPTION_EVENT == 1+GENERAL_SUCCESS_EVENT
  OperationFailedException = GENERAL_EXCEPTION_EVENT,
  // for ASYNCH send code
  ChannelBufferFullException,                 // thrown by send when local (and connected channel) queue buffers are full
  DivisionByZeroException,
  // For inter-node channels 
  SendStatusUnknownException,                 // status of of SYNCH send over inter-node channel is unknown, no ACK received
  DuplicateLCNException,                      // two channels published with the same LCN on a single node
  NodesUnreachableException,                  // there is no route to the remote node 
  ChannelsUnknownException,                   // the specified remote channel has not been published on the remote node 
  IncompatibleChannelsException,              // two channels are incomaptible for binding to take place
  BindStatusUnknownException                  // status of binding two inter-node channels together is unknown, no ACK received
} inceos_exception_t;


// functions used to support exception handling

// returns true when the event is an exception event
bool is_exception_event(inceos_event_t ev);


#endif /* EVENTS_H */
