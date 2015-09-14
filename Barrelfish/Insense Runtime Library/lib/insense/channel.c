/**
 *
 * Start of channel implementation for UNIX
 *
 * Send, receive, bind, unbind are implemented according to SPIN paper algorithms
 * Select operation is missing
 *
 * Absolutely no guarantees with this code, nevcer been tested by me
 *
 * @author jonl, based on code from Andrew Bell
 *
 * 26.04.2013
 *
 */

#include <insense/channel.h>
#include <insense/cstring.h>

pthread_mutex_t conn_op_mutex = PTHREAD_MUTEX_INITIALIZER;	// to prevent connect during disconnect and vice versa

static void Channel_decRef(Channel_PNTR pntr);
static char *file_name = "Channel";

#if MCDEBUG
#define PRINTFMC(...) printf(__VA_ARGS__)
#else
#define PRINTFMC(...)
#endif

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
#ifndef NULL
#define NULL   ((void *) 0)
#endif


static void initialise_sems_and_mutexes(Channel_PNTR this){
	// Initialise mutexes and semaphores
	pthread_mutex_init(&(this->mutex), NULL);
	my_sem_init(&(this->conns_sem), 0);
	my_sem_init(&(this->blocked), 0);
	my_sem_init(&(this->actually_received), 0);
}

Channel_PNTR channel_create(chan_dir direction, int typesize, bool contains_pointers) {
	PRINTFMC("Create channel\n");
	Channel_PNTR this = (Channel_PNTR)DAL_alloc(sizeof(struct Channel), true);
	if(this == (void*) 0){
		DAL_error(CHAN_OUT_OF_MEMORY_ERROR);
		return NULL;
	}

	this->decRef = Channel_decRef;
	this->direction = direction;
	this->typesize = typesize;
	this->ready = false;
	this->nd_received = false;
	DAL_assign(&(this->connections), Construct_List());	// empty list of connections

	initialise_sems_and_mutexes(this);

	// Setup the stopped condition

	return(this);
}


void Channel_decRef(Channel_PNTR this){
	channel_unbind(this);                   // disconnect from all other chans
	DAL_decRef(this->connections);          // GC connections list
	pthread_mutex_destroy(&(this->mutex));
	my_sem_destroy( &(this->conns_sem) );		// now destroy mutexes and semaphores
	my_sem_destroy( &(this->blocked) );
	my_sem_destroy( &(this->actually_received) );
}


bool channel_bind(Channel_PNTR id1, Channel_PNTR id2) {
	PRINTFMC("Bind channels ID1: %d and ID2: %d\n", id1, id2);
	pthread_mutex_lock(&conn_op_mutex);
	
	// check not both CHAN_IN or CHAN_OUT
	if(id1->direction == id2->direction) {
		PRINTFMC("Bind directions are the same\n");
		pthread_mutex_unlock(&conn_op_mutex);
		return false;
	} 
	if (id1->typesize != id2->typesize) {
		PRINTFMC("Bind typesizes are different\n");
		pthread_mutex_unlock(&conn_op_mutex);
		return false;
	}

	pthread_mutex_lock(id1->direction == CHAN_IN ? &(id1->mutex) : &(id2->mutex));
	pthread_mutex_lock(id1->direction == CHAN_IN ? &(id2->mutex) : &(id1->mutex));

	// check not already connected
	// assuming bind always adds to both channels' lists, we only need to check one channel for the other
	if(containsElement(id1->connections, (void*)id2)) {
		pthread_mutex_unlock(&conn_op_mutex);
		return false;
	}

	// add to conns lists
	insertElement(id1->connections, id2);
	insertElement(id2->connections, id1);
	// JL to counter garbage collection code in insertElement
	DAL_modRef_by_n(id1, -1);
	DAL_modRef_by_n(id2, -1);

	// unlock conns mutex in both channels
	// never allow semaphores to go above 1; make it act like a mutex or binary semaphore
	binary_sem_post(&(id1->conns_sem)); // do post to value 1
	binary_sem_post(&(id2->conns_sem));
	
	pthread_mutex_unlock(&(id1->mutex));
	pthread_mutex_unlock(&(id2->mutex));

	pthread_mutex_unlock(&conn_op_mutex);
	return true;
}

void channel_unbind(Channel_PNTR id) {
	pthread_mutex_lock(&conn_op_mutex);

	// iterate through list, locking then disconnecting
	unsigned int length = getListLength(id->connections);
	Channel_PNTR opposite; // channel on the opposite side of current connection
	int i;
	for(i = 0; i < length; i++) {
		opposite = getElementN(id->connections, i);	// fetch current opposite half-channel

		binary_sem_wait(id->direction == CHAN_IN ? &(id->conns_sem) : &(opposite->conns_sem) );
		binary_sem_wait(id->direction == CHAN_IN ? &(opposite->conns_sem) : &(id->conns_sem) );
		pthread_mutex_lock(id->direction == CHAN_IN ? &(id->mutex) : &(opposite->mutex) );
		pthread_mutex_lock(id->direction == CHAN_IN ? &(opposite->mutex) : &(id->mutex) );

		// JL to counter garbage collection code in removeElement
		DAL_modRef_by_n(id, 2);
		DAL_modRef_by_n(opposite, 2);
		removeElement(id->connections, opposite);
		removeElement(opposite->connections, id);
		DAL_modRef_by_n(id, -1);
		DAL_modRef_by_n(opposite, -1);

		pthread_mutex_unlock(&(id->mutex));
		pthread_mutex_unlock(&(opposite->mutex));

		if(!isEmpty(id->connections)) {
			binary_sem_post(&(id->conns_sem));
		}
		if(!isEmpty(opposite->connections)){
			binary_sem_post(&(opposite->conns_sem));
		}
		length = getListLength(id->connections);
	}

	pthread_mutex_unlock(&conn_op_mutex);
	return;
}

int channel_select(struct select_struct *s) {
	return 0;
}

int channel_send(Channel_PNTR cout, void *data, void *ex_handler) {
	binary_sem_wait(&(cout->conns_sem));
	pthread_mutex_lock(&(cout->mutex));

	cout->buffer = data;
	cout->ready = true;
	pthread_mutex_unlock(&(cout->mutex));

	// iterate through connection list, looking for receiver that is ready
	unsigned int length = getListLength(cout->connections);
	Channel_PNTR match; // current receiver
	int i;
	for(i = 0; i < length; i++) {
		match = getNextElement(cout->connections);	// fetch next channel in conns list (which keeps state across calls)

		pthread_mutex_lock(&(match->mutex));
		pthread_mutex_lock(&(cout->mutex));

		if(match->ready && cout->ready) {
			match->buffer = cout->buffer;
			match->ready = false;
			cout->ready = false;
			cout->nd_received = true;
			my_sem_post(&(match->blocked));
			pthread_mutex_unlock(&(cout->mutex));
			pthread_mutex_unlock(&(match->mutex));
			my_sem_wait(&(match->actually_received));

			// unlock conns semaphore
			binary_sem_post(&(cout->conns_sem)); // do post to value 1

			return 0;
		}

		pthread_mutex_unlock(&(cout->mutex));
		pthread_mutex_unlock(&(match->mutex));
	}

	// unlock conns semaphore
	binary_sem_post(&(cout->conns_sem)); // do post to value 1
	my_sem_wait(&(cout->blocked));	// blocked should always be locked, so wait here until data is taken by active part of a receive
	return 0;
}

int channel_receive(Channel_PNTR cin, void *data, bool in_ack_after) {
	binary_sem_wait(&(cin->conns_sem));
	pthread_mutex_lock(&(cin->mutex));

	cin->ready = true;

	pthread_mutex_unlock(&(cin->mutex));

	// iterate through connection list, looking for receiver that is ready
	unsigned int length = getListLength(cin->connections);
	Channel_PNTR match; // current receiver
	int i;
	for(i = 0; i < length; i++) {
		match = getNextElement(cin->connections);	// fetch next channel in conns list (which keeps state across calls)

		pthread_mutex_lock(&(cin->mutex));
		pthread_mutex_lock(&(match->mutex));

		if(match->ready && cin->ready) {
			cin->buffer = match->buffer;		// found a ready sender, get pointer
			memncpy(data, cin->buffer, cin->typesize);	// got pointer from sender; copy data

			match->ready = false;
			cin->ready = false;

			my_sem_post(&(match->blocked));
			pthread_mutex_unlock(&(match->mutex));
			pthread_mutex_unlock(&(cin->mutex));

			// unlock conns semaphore
			binary_sem_post(&(cin->conns_sem)); // do post to value 1

			return 0;
		}

		pthread_mutex_unlock(&(match->mutex));
		pthread_mutex_unlock(&(cin->mutex));
	}

	// unlock conns semaphore
	binary_sem_post(&(cin->conns_sem)); // do post to value 1
	my_sem_wait(&(cin->blocked) );	// wait here until data is ready in active part of a send

	memncpy(data, cin->buffer, cin->typesize);	// receiver now has pointer; copy data
	my_sem_post(&(cin->actually_received) );

	return 0;
}
