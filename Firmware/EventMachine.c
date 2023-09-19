#include "EventMachine.h"

//Initialize the event machine
void event_machine_init(evtmachine_t* machine, event_handler handler, uint8_t event_size, uint8_t queue_depth)
{
    queue_init(&machine->queue, event_size, queue_depth);
    machine->handler = handler;
}

//Adds an event to the machine
void event_push(evtmachine_t* machine, void* event)
{
    queue_add_blocking(&machine->queue, event);
}

//Processes the pending events
void event_process_queue(evtmachine_t* machine, void* event_buffer, uint8_t max_events)
{
    uint8_t evt_count = 0;
    while(!queue_is_empty(&machine->queue) && evt_count++ < max_events)
    {
        queue_remove_blocking(&machine->queue, event_buffer);
        machine->handler(event_buffer);
    }
}

//Clears the stored events in the machine
void event_clear(evtmachine_t* machine)
{
    machine->queue.wptr = 0;
    machine->queue.rptr = 0;
}

//Free an event machine
void event_free(evtmachine_t* machine)
{
    queue_free(&machine->queue);
    machine->handler = NULL;
}