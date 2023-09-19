#ifndef __EVENTMACHINE__
#define __EVENTMACHINE__

#include "pico/stdlib.h"
#include "pico/util/queue.h"

//Event handler function declaration
typedef void(*event_handler)(void*);

//Event machine struct
typedef struct evtmachine
{
    //Queue to store events
    queue_t queue;
    //Function to process the events
    event_handler handler;

} evtmachine_t;

void event_machine_init(evtmachine_t* machine, event_handler handler, uint8_t args_size, uint8_t queue_depth);
void event_push(evtmachine_t* machine, void* event);
void event_process_queue(evtmachine_t* machine, void* event_buffer, uint8_t max_events);
void event_clear(evtmachine_t* machine);
void event_free(evtmachine_t* machine);

#endif