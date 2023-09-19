#include <stdio.h>
#include "EventMachine.h"
#include "SharedBuffers.h"
/*

Buffers for the microdrive PIO machines
We have two sets so we can go ahead before the user interface core
processes the received data or writes new sectors to it

*/

//First set of buffers
//Header of a sector, two tracks
uint8_t header_1_track_1[HEADER_BUFFER_SIZE];
uint8_t header_1_track_2[HEADER_BUFFER_SIZE];

//Content of a sector, two tracks
uint8_t sector_1_track_1[SECTOR_BUFFER_SIZE];
uint8_t sector_1_track_2[SECTOR_BUFFER_SIZE];
uint8_t bufferset_1_sector_number = 0;

//Second set of buffers
//Header of a sector, two tracks
uint8_t header_2_track_1[HEADER_BUFFER_SIZE];
uint8_t header_2_track_2[HEADER_BUFFER_SIZE];

//Content of a sector, two tracks
uint8_t sector_2_track_1[SECTOR_BUFFER_SIZE];
uint8_t sector_2_track_2[SECTOR_BUFFER_SIZE];
uint8_t bufferset_2_sector_number = 0;

//Cartridge image buffer
uint8_t cartridge_image[CART_SIZE];

/*
Event machines
*/

//Machine processed by the ui control, receives events from the md
evtmachine_t mdToUiEventQueue;
//Machine processed by the md control, receives events from the ui
evtmachine_t uiToMdEventQueue;