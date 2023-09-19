#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/dma.h"
#include "hardware/timer.h"
#include "MicroDriveControl.h"
#include "SharedBuffers.h"
#include "SharedEvents.h"
#include "EventMachine.h"
#include "PIO_machines.pio.h"

bool isCartridgeInserted = false;
bool secondBufferSet = false;

mdstatus_t mdStatus = MDS_DESELECTED;
mdactivestatus_t activeStatus = MDA_IDLE;

evtmachine_t mdEventQueue;

uint track1DMA;
uint track2DMA;

bool track1DMAFired = false;
bool track2DMAFired = false;

dma_channel_config track1ReadConfig;
dma_channel_config track2ReadConfig;

dma_channel_config track1WriteConfig;
dma_channel_config track2WriteConfig;

dma_channel_config track1DisabledConfig;
dma_channel_config track2DisabledConfig;

uint rxProgramOffset;
uint statProgramOffset;
uint txProgramOffset;
uint shiftProgramOffset;

int sm_read_head_1;
int sm_read_head_2;
int sm_status;
int sm_exec_read;

int sm_write_head_1;
int sm_write_head_2;
int sm_shifter;
int sm_exec_write;

//enable the microdrive
void select_md()
{
    //Change the md status to active
    mdStatus = MDS_SELECTED;

    //Enable the status machine
    select_PIO_status();

    //Notify to the UI
    mtuevent_t evt;
    evt.event = MTU_MD_SELECTED;
    event_push(&mdToUiEventQueue, &evt);
}

//disable the microdrive
void deselect_md()
{
    //Reset the select status
    mdStatus = MDS_DESELECTED;

    //We store the previous state to check if we need to notify the UI about any buffer change
    mdactivestatus_t previousState = activeStatus;

    //We reset the active status, this may cause the last buffer that was being read or written
    //to be sent again to the ULA if it was a header, but this should not cause any trouble as the QL
    //does not deselect the drive while it's doing a write/format operation
    activeStatus = MDA_IDLE;

    //Abort the alarms if pending
    abort_shifter_alarm();
    abort_write_gap_alarm();

    //Ensure both transfer machines are idle
    sleep_PIO_write();
    begin_PIO_read_gap();

    //Disable the status machine
    deselect_PIO_status();
    
    //Disable the DMAs if they're active
    if(activeStatus >= MDA_READ_HEADER || activeStatus <= MDA_READ_SECTOR)
        disable_DMAs(true);
    else if(activeStatus >= MDA_WRITE_HEADER_GAP || activeStatus <= MDA_WRITE_SECTOR)
        disable_DMAs(false);

    //Set dir to input to avoid problems
    gpio_put(MD_HEAD_DIR, 1);

    //Check for any buffer change. This is mostly by sanity, the MD should not deselect the device while it's
    //writting to the md and if we send again the same sector to the ULA it will skip it if needed
    check_ui_notifications(previousState, false);

    //Notify to the UI about the device deselection
    mtuevent_t evt;
    evt.event = MTU_MD_DESELECTED;
    event_push(&mdToUiEventQueue, &evt);
}

//This function checks if the UI needs to be notified about any buffer change event
//This is called at the beginning of the gaps and when the device is deselected
void check_ui_notifications(mdactivestatus_t previousState, bool fromGap)
{
    switch(previousState)
    {
        case MDA_READ_SECTOR:   //We were reading a sector from the ULA, notify the UI that it needs to write the buffers to the cartridge
                                //image and fill the buffers with the next sector
        {
            mtuevent_t readEvt;
            readEvt.event = MTU_BUFFERSET_WRITTEN;
            //If we are called from a gap routine the buffer set has already been changed
            //but if we are being called from the deselection method then the bufferset has not been changed.
            if(fromGap)
                readEvt.arg = secondBufferSet ? 0 : 1;
            else
                readEvt.arg = secondBufferSet ? 1 : 0;

            event_push(&mdToUiEventQueue, &readEvt);
        }
            break;

        case MDA_WRITE_SECTOR:  //We were writting a sector to the ULA, notify the UI that it needs to fill the buffers with the next sector
        {
            mtuevent_t writeEvt;
            writeEvt.event = MTU_BUFFERSET_READ;
            //If we are called from a gap routine the buffer set has already been changed
            //but if we are being called from the deselection method then the bufferset has not been changed.
            if(fromGap)
                writeEvt.arg = secondBufferSet ? 0 : 1;
            else
                writeEvt.arg = secondBufferSet ? 1 : 0;

            event_push(&mdToUiEventQueue, &writeEvt);
        }

            break;

    }

    switch(activeStatus)
    {
        case MDA_READ_HEADER_GAP:
        case MDA_READ_SECTOR_GAP:
        {
            mtuevent_t readEvt;
            readEvt.event = MTU_MD_READING;
            event_push(&mdToUiEventQueue, &readEvt);
        }
            break;

        case MDA_WRITE_HEADER_GAP:
        case MDA_WRITE_SECTOR_GAP:
        {
            mtuevent_t writeEvt;
            writeEvt.event = MTU_MD_WRITTING;
            event_push(&mdToUiEventQueue, &writeEvt);
        }
            break;
    }
}

//This code is always executed in any gap start, resets the PIO machines, checks for buffer set changes
//Returns if the next buffer is a header or a sector (true = header, false = sector)
//Sets which set of buffers we're going to use
bool common_gap_code(uint8_t** selectedTrack1Buffer, uint8_t** selectedTrack2Buffer, bool forRead)
{
    //Abort any pending write gap alarm
    abort_write_gap_alarm();

    //Send the TX machine to sleep if this is a RX, else send it to the gap
    if(forRead)
        sleep_PIO_write();
    else
        begin_PIO_write_gap();

    begin_PIO_read_gap();

    //Abort the DMA's if they were active
    if(activeStatus >= MDA_READ_HEADER_GAP && activeStatus <= MDA_READ_SECTOR)
        disable_DMAs(true);
    else if(activeStatus >= MDA_WRITE_HEADER_GAP && activeStatus <= MDA_WRITE_SECTOR)
        disable_DMAs(false);

    //Depending on our previous state this will read/write a header or a sector
    //When the QL formats a cartridge it writes the header and the sector, but when it
    //writes data it will read the header and then write the sector so we
    //must ensure that we don't restart at a header at the incorrect place

    //IDLE -> header
    //READ_HEADER_GAP -> header
    //READ_HEADER -> sector
    //READ_SECTOR_GAP -> sector
    //READ_SECTOR -> header
    //WRITE_HEADER_GAP -> header
    //WRITE_HEADER -> sector
    //WRITE_SECTOR_GAP -> sector
    //WRITE_SECTOR -> header

    bool isHeader;

    switch(activeStatus)
    {
        case MDA_IDLE:
        case MDA_READ_HEADER_GAP:
        case MDA_READ_SECTOR:
        case MDA_WRITE_HEADER_GAP:
        case MDA_WRITE_SECTOR:

            isHeader = true;
            break;

        case MDA_READ_HEADER:
        case MDA_READ_SECTOR_GAP:
        case MDA_WRITE_HEADER:
        case MDA_WRITE_SECTOR_GAP:

            isHeader = false;
            break;
    }

    //Lets check if we need to switch buffers
    if(isHeader)
    {
        switch(activeStatus)
        {
            //If we were reading or writting a sector then the next is a header and we need to change the buffer set
            case MDA_READ_SECTOR:
            case MDA_WRITE_SECTOR:
                secondBufferSet = !secondBufferSet;
                break;
        }

        //Choose the correct buffers
        if(secondBufferSet)
        {
            *selectedTrack1Buffer = header_2_track_1;
            *selectedTrack2Buffer = header_2_track_2;
        }
        else
        {
            *selectedTrack1Buffer = header_1_track_1;
            *selectedTrack2Buffer = header_1_track_2;
        }
    }
    else
    {
        //Choose the correct buffers
        if(secondBufferSet)
        {
            *selectedTrack1Buffer = sector_2_track_1;
            *selectedTrack2Buffer = sector_2_track_2;
        }
        else
        {
            *selectedTrack1Buffer = sector_1_track_1;
            *selectedTrack2Buffer = sector_1_track_2;
        }
    }
    return isHeader;
}

//Start the write gap process, prepares all the buffers and DMAs and notifies to the UI if needed
void begin_write_gap()
{
    //We entered in a read gap, because we have finished sending a sector or we're starting to send data to the ULA

    //Pointers to the final buffers
    uint8_t* track1Buffer;
    uint8_t* track2Buffer;

    //Execute the common code to the gaps and get if the next that needs to be read is a header or a sector
    bool isHeader = common_gap_code(&track1Buffer, &track2Buffer, false);
    mdactivestatus_t previousState = activeStatus;
    
    //Set our new active status
    if(isHeader)
        activeStatus = MDA_WRITE_HEADER_GAP;
    else
        activeStatus = MDA_WRITE_SECTOR_GAP;

    //Clear any pending data in the FIFOs
    pio_sm_clear_fifos(pio1, sm_write_head_1);
    pio_sm_clear_fifos(pio1, sm_write_head_2);

    //Enable the write DMA's. This will send the buffers data to the ULA
    //once the read machines are triggered
    enable_write_DMAs(track1Buffer, track2Buffer, isHeader);

    //Start the write gap timeout
    begin_write_gap_alarm();

    //Set dir to output
    gpio_put(MD_HEAD_DIR, 0);

    //Check if we need to notify anything to the UI
    check_ui_notifications(previousState, true);
}

//End the read gap, this starts writting a buffer set to the ULA
void end_write_gap()
{
     //Change our active state to the proper one
    switch(activeStatus)
    {
        case MDA_WRITE_HEADER_GAP:
            activeStatus = MDA_WRITE_HEADER;
            break;
        case MDA_WRITE_SECTOR_GAP:
            activeStatus = MDA_WRITE_SECTOR;
            break;
        default:
            return;
    }

    //Enable the PIO write machines
    end_PIO_write_gap();
}

//Start the read gap process, prepares all the buffers and DMAs and notifies to the UI if needed
void begin_read_gap()
{
    //The ULA entered in a write gap (ERASE enabled and WRITE off)

    //Pointers to the final buffers
    uint8_t* track1Buffer;
    uint8_t* track2Buffer;

    //Execute the common code to the gaps and get if the next that needs to be read is a header or a sector
    bool isHeader = common_gap_code(&track1Buffer, &track2Buffer, true);
    mdactivestatus_t previousState = activeStatus;
    
    //Set our new active status
    if(isHeader)
        activeStatus = MDA_READ_HEADER_GAP;
    else
        activeStatus = MDA_READ_SECTOR_GAP;

    //Clear any pending data in the FIFOs
    pio_sm_clear_fifos(pio0, sm_read_head_1);
    pio_sm_clear_fifos(pio0, sm_read_head_2);

    //Enable the read DMA's. This will fill the buffers with data from the ULA
    //once the read machines are triggered
    enable_read_DMAs(track1Buffer, track2Buffer, isHeader);

    //Set dir to input
    gpio_put(MD_HEAD_DIR, 1);

    //Check if we need to notify anything to the UI
    check_ui_notifications(previousState, true);
    
}

//End the read gap, this starts reading a buffer set from the ULA
void end_read_gap()
{
    //The ULA started writing so we enable the read machines (only if we were previously in a read gap, needs to be tested on file writes)
    
    //There are no concerns about timmings, we run at 200Mhz so we should be able to catch every single
    //bit without losing any, and even in the case that by any chance we miss a bit it will not be a problem
    //because the microdrive/ULA sends a calibration preamble that will be discarded by the UI processing
    //mechanism.

    //Change our active state to the proper one
    switch(activeStatus)
    {
        case MDA_READ_HEADER_GAP:
            activeStatus = MDA_READ_HEADER;
            break;
        case MDA_READ_SECTOR_GAP:
            activeStatus = MDA_READ_SECTOR;
            break;
        default:
            return;
    }

    //Enable the PIO read machines
    end_PIO_read_gap();
}

//Shifter selection alarm expired
void shifter_alarm(uint alarm_num)
{
    abort_shifter_alarm();

    mdcontrolevent_t event;
    event.event = MDE_SELECT_TIMEOUT_EXPIRED;
    event_push(&mdEventQueue, &event);
}

//Write gap end alarm
void write_gap_alarm(uint alarm_num)
{
    //Abort the alarm
    abort_write_gap_alarm();

    //If our previous state is not a write gap we ignore the alarm, for sanity
    if(activeStatus != MDA_WRITE_HEADER_GAP && activeStatus != MDA_WRITE_SECTOR_GAP)
        return;

    //Create the event
    mdcontrolevent_t evtGap;
    evtGap.event = MDE_WRITE_GAP_FINISHED;
    event_push(&mdEventQueue, &evtGap);
}

//Function to cancel the shifter alarm
static inline void abort_shifter_alarm()
{
    hardware_alarm_cancel(SHIFTER_ALARM);
}

//Function to cancel the write gap alarm
static inline void abort_write_gap_alarm()
{
    hardware_alarm_cancel(WRITE_GAP_ALARM);
}

//Function to start the shifter selected alarm
static inline void begin_shifter_alarm()
{
    hardware_alarm_set_target(SHIFTER_ALARM, delayed_by_us(get_absolute_time(), SHIFTER_SELECT_US));
}

//Function to start the write gap alarm
static inline void begin_write_gap_alarm()
{
    hardware_alarm_set_target(WRITE_GAP_ALARM, delayed_by_us(get_absolute_time(), QL_WRITE_GAP_US));
}

//Lets the shifter PIO machine to run
static inline void start_PIO_shifter()
{
    pio_sm_exec(pio1, sm_exec_write, pio_encode_irq_set(false, 6));
}

//Sends the status machine to the deselected state
static inline void deselect_PIO_status()
{
    pio_sm_exec(pio0, sm_status, pio_encode_jmp(statProgramOffset + microdrive_status_offset_device_deselected));
}

//Sends the status machine to the selected state
static inline void select_PIO_status()
{
    pio_sm_exec(pio0, sm_exec_read, pio_encode_irq_set(false, 6));
}

//Send read machines to the gap state
static inline void begin_PIO_read_gap()
{
    pio_sm_exec(pio0, sm_exec_read, pio_encode_irq_set(false, 7));
    pio_sm_exec(pio0, sm_read_head_1, pio_encode_jmp(rxProgramOffset + microdrive_read_offset_rx_gap));
    pio_sm_exec(pio0, sm_read_head_2, pio_encode_jmp(rxProgramOffset + microdrive_read_offset_rx_gap));
}

//Exit read machines from the gap state
static inline void end_PIO_read_gap()
{
    pio_interrupt_clear(pio0, 7);
    //pio_sm_exec(pio0, sm_exec_read, pio_encode_irq_clear(false, 7));
}

//Put write PIO machine to sleep
static inline void sleep_PIO_write()
{
    pio_sm_exec(pio1, sm_exec_write, pio_encode_irq_set(false, 7));
    pio_sm_exec(pio1, sm_write_head_1, pio_encode_jmp(txProgramOffset + microdrive_write_offset_tx_sleep));
    pio_sm_exec(pio1, sm_write_head_2, pio_encode_jmp(txProgramOffset + microdrive_write_offset_tx_sleep));
}

//Send write machines to the gap state
static inline void begin_PIO_write_gap()
{
    pio_sm_exec(pio1, sm_exec_write, pio_encode_irq_set(false, 7));
    pio_sm_exec(pio1, sm_write_head_1, pio_encode_jmp(txProgramOffset + microdrive_write_offset_tx_gap));
    pio_sm_exec(pio1, sm_write_head_2, pio_encode_jmp(txProgramOffset + microdrive_write_offset_tx_gap));
}

//Exit write machines from the gap state
static inline void end_PIO_write_gap()
{
    pio_interrupt_clear(pio1, 7);
    //pio_sm_exec(pio1, sm_exec_write, pio_encode_irq_clear(false, 7));
}

//Handler to process events related to the microdrive
void process_md_event(void* event)
{
    mdcontrolevent_t* mdevt = (mdcontrolevent_t*)event;
    
    switch(mdevt->event)
    {
        case MDE_SHIFT_CHANGED:

            //The shifter has received a zero
            if(mdevt->args == 0)
            {
                //Abort the shifter alarm in case it was active
                abort_shifter_alarm();

                //Ignore the deselection if we aren't selected
                if(mdStatus == MDS_SELECTED)
                    deselect_md(); //Deselect us
            }
            else
            {
                //The shifter has received an one
                
                //Ignore it if we were selected
                if(mdStatus == MDS_DESELECTED)
                    begin_shifter_alarm();
            }

            break;

        case MDE_SELECT_TIMEOUT_EXPIRED:

            //only select the device if this is deselected
            if(mdStatus == MDS_DESELECTED)
                select_md();

            break;

        case MDE_MD_STATUS_CHANGED:

            //If the device is not selected or there is no cartridge we ignore these, this will effectively block any kind of operation in the md
            if(mdStatus != MDS_SELECTED || !isCartridgeInserted)
                return;

            switch((mdlinestatus_t)mdevt->args)
            {
                case MDL_READ: //ULA wants to read from us

                    begin_write_gap(); //Write to the ULA
                    break;
                
                case MDL_WRITE_GAP: //ULA entered a write gap

                    begin_read_gap();
                    break;

                case MDL_WRITE: //ULA exited from a write gap and entered in a real write

                    end_read_gap();
                    break;

            }

            break;

        case MDE_WRITE_GAP_FINISHED:

            //Finalize the write gap
            end_write_gap();
            break;

        case MDE_DMA_WRITE_IRQ:

            //Set the trigger flags
            if(mdevt->args == 1)
                track1DMAFired = true;
            else
                track2DMAFired = true;

            //Have both tracks finished writting?
            if(track1DMAFired && track2DMAFired)
            {
                if(!pio_sm_is_tx_fifo_empty(pio1, sm_write_head_1) || !pio_sm_is_tx_fifo_empty(pio1, sm_write_head_2))
                {
                    mdcontrolevent_t checkEvt;
                    checkEvt.event = MDE_CHECK_WRITE_FINISH;
                    event_push(&mdEventQueue, &checkEvt);
                    break;
                }

                begin_write_gap(); //Start a new gap
            }

            break;

        case MDE_CHECK_WRITE_FINISH:

            if(!pio_sm_is_tx_fifo_empty(pio1, sm_write_head_1) || !pio_sm_is_tx_fifo_empty(pio1, sm_write_head_2))
            {
                event_push(&mdEventQueue, mdevt);
                break;
            }

            begin_write_gap(); //Start a new gap

            break;

    }

}

//Handler to process events received from the UI
void process_ui_event(void* event)
{
    utmevent_t* mduievt = (utmevent_t*)event;

    switch (mduievt->event)
    {
        case UTM_CARTRIDGE_INSERTED:
            
            isCartridgeInserted = true;
            activeStatus = MDA_IDLE; //The deselect should have reset this, we do it just for sanity
            secondBufferSet = false; //Reset the active buffer set, we do it just for sanity
            track1DMAFired = false;
            track2DMAFired = false;
            
            break;
    
        case UTM_CARTRIDGE_REMOVED:
            
            isCartridgeInserted = false;
            
            reset_transfer_machines();  //Reset the PIO TX/RX machines
            abort_write_gap_alarm();    //Abort any pending write alarm
            disable_DMAs(true);         //Disable all DMA transfers
            disable_DMAs(false);
            gpio_put(MD_HEAD_DIR, 1);   //Set dir to input, for sanity

            activeStatus = MDA_IDLE;    //we start in the idle status
            secondBufferSet = false;    //Reset the active buffer set
            track1DMAFired = false;     //This is already called in disable_dma, left for sanity
            track2DMAFired = false;

            break;
    }
    
}

//The PIO machine has finished sending data to the ULA.
void track1_write_irq()
{
    //Clear the interrupt flag
    dma_channel_acknowledge_irq0(track1DMA);
    //Create irq event
    mdcontrolevent_t writeEvent;
    writeEvent.event = MDE_DMA_WRITE_IRQ;
    writeEvent.args = 1; //Indicates the track number
    event_push(&mdEventQueue, &writeEvent);
}

//The PIO machine has finished sending data to the ULA.
void track2_write_irq()
{
    //Clear the interrupt flag
    dma_channel_acknowledge_irq1(track2DMA);
    //Create irq event
    mdcontrolevent_t writeEvent;
    writeEvent.event = MDE_DMA_WRITE_IRQ;
    writeEvent.args = 2; //Indicates the track number
    event_push(&mdEventQueue, &writeEvent);
}

//Notification from the PIO about a status change
void status_irq()
{
    //Clear the interrupt flag
    pio_interrupt_clear(pio0, 0);

    //Read the status from the machine's FIFO
    uint8_t newMdStatus = pio_sm_get(pio0, sm_status);

    if(newMdStatus == MDL_INVALID)
        return;

    //Create event and push it to the queue
    //Consider go back to a static or predefined event.
    mdcontrolevent_t statusEvent;
    statusEvent.event = MDE_MD_STATUS_CHANGED;
    statusEvent.args = newMdStatus;
    event_push(&mdEventQueue, &statusEvent);
}

//Notification from the PIO about the shifter change
void shifter_irq()
{
    //Clear the interrupt flag
    pio_interrupt_clear(pio1, 0);

    //Read the shifter status from the machine's FIFO
    uint8_t newSelStatus = pio_sm_get(pio1, sm_shifter);

    //Create event and push it to the queue
    //Consider go back to a static or predefined event.
    mdcontrolevent_t shifterEvent;
    shifterEvent.event = MDE_SHIFT_CHANGED;
    shifterEvent.args = newSelStatus;
    event_push(&mdEventQueue, &shifterEvent);
}

//Initialize the DMA channels and preconfigure the config structures
void init_DMAs()
{
    //Claim two DMA's, one for each track
    track1DMA = dma_claim_unused_channel(true);
    track2DMA = dma_claim_unused_channel(true);

    //Enable IRQ's in the NVIC
    irq_set_enabled (DMA_IRQ_0, true);
    irq_set_enabled (DMA_IRQ_1, true);

    //configure the track1 read config
    track1ReadConfig = dma_channel_get_default_config(track1DMA);
    channel_config_set_dreq(&track1ReadConfig, pio_get_dreq(pio0, sm_read_head_1, false));
    channel_config_set_read_increment(&track1ReadConfig, false);
    channel_config_set_write_increment(&track1ReadConfig, true);
    channel_config_set_transfer_data_size(&track1ReadConfig, DMA_SIZE_8);

    //configure the track2 read config
    track2ReadConfig = dma_channel_get_default_config(track2DMA);
    channel_config_set_dreq(&track2ReadConfig, pio_get_dreq(pio0, sm_read_head_2, false));
    channel_config_set_read_increment(&track2ReadConfig, false);
    channel_config_set_write_increment(&track2ReadConfig, true);
    channel_config_set_transfer_data_size(&track2ReadConfig, DMA_SIZE_8);

    //configure the track1 write config
    track1WriteConfig = dma_channel_get_default_config(track1DMA);
    channel_config_set_dreq(&track1WriteConfig, pio_get_dreq(pio1, sm_write_head_1, true));
    channel_config_set_read_increment(&track1WriteConfig, true);
    channel_config_set_write_increment(&track1WriteConfig, false);
    channel_config_set_transfer_data_size(&track1WriteConfig, DMA_SIZE_8);

    //configure the track2 write config
    track2WriteConfig = dma_channel_get_default_config(track2DMA);
    channel_config_set_dreq(&track2WriteConfig, pio_get_dreq(pio1, sm_write_head_2, true));
    channel_config_set_read_increment(&track2WriteConfig, true);
    channel_config_set_write_increment(&track2WriteConfig, false);
    channel_config_set_transfer_data_size(&track2WriteConfig, DMA_SIZE_8);

    track1DisabledConfig = dma_channel_get_default_config(track1DMA);
    channel_config_set_enable(&track1DisabledConfig, false);

    track2DisabledConfig = dma_channel_get_default_config(track2DMA);
    channel_config_set_enable(&track2DisabledConfig, false);
}

//Enable DMAs to receive data from the PIO machines
void enable_read_DMAs(uint8_t* buffer_track_1, uint8_t* buffer_track_2, bool isHeader)
{

    //Do we really need read DMA's? the read operation will be finished by the status change
    //Needs to be tested

    //No, we don't need (and can't use) DMA IRQ's as the ULA will send trash in the preamble and we can't
    //warantee that we have finished the exact transfer of bits, also the ULA will start a write gap
    //after all the bits have been written, the 

    /*
    //Set IRQ handlers
    irq_set_exclusive_handler(DMA_IRQ_0, track1_read_irq);
    irq_set_exclusive_handler(DMA_IRQ_1, track2_read_irq);

    //Enable IRQ's
    dma_channel_set_irq0_enabled(track1DMA, true);
    dma_channel_set_irq1_enabled(track2DMA, true);
    */

    //Configure and start channels
    dma_channel_configure(track1DMA, &track1ReadConfig, buffer_track_1, &pio0->rxf[sm_read_head_1], isHeader? 
        HEADER_BUFFER_SIZE : SECTOR_BUFFER_SIZE, true);
    dma_channel_configure(track2DMA, &track2ReadConfig, buffer_track_2, &pio0->rxf[sm_read_head_2], isHeader? 
        HEADER_BUFFER_SIZE : SECTOR_BUFFER_SIZE, true);
}

//Enable DMAs to send data to the PIO machines
void enable_write_DMAs(uint8_t* buffer_track_1, uint8_t* buffer_track_2, bool isHeader)
{
    //Set IRQ handlers
    irq_set_exclusive_handler(DMA_IRQ_0, track1_write_irq);
    irq_set_exclusive_handler(DMA_IRQ_1, track2_write_irq);

    //Enable IRQ's
    dma_channel_set_irq0_enabled(track1DMA, true);
    dma_channel_set_irq1_enabled(track2DMA, true);

    //We regenerate the preamble and the data so we transfer perfect bits, we don't need to care about the ULA
    //trash in the preambles or at the end of the data, our transfers always have the same size (the buffers are)
    //prepared with the 4 bit displacements for track2 and the four trashs bit in the tail.

    //Configure and start channels
    dma_channel_configure(track1DMA, &track1WriteConfig, &pio1->txf[sm_write_head_1], buffer_track_1, isHeader? 
        (QL_PREAMBLE_SIZE + QL_HEADER_SIZE) : (QL_PREAMBLE_SIZE + QL_SECTOR_SIZE), true);
    dma_channel_configure(track2DMA, &track2WriteConfig, &pio1->txf[sm_write_head_2], buffer_track_2, isHeader? 
        (QL_PREAMBLE_SIZE + QL_HEADER_SIZE) : (QL_PREAMBLE_SIZE + QL_SECTOR_SIZE), true);
}

//Disable DMAs. Should be called in gaps, status changes and shifter changes.
void disable_DMAs(bool readDMAs)
{
    //Reset the fired flags
    track1DMAFired = false;
    track2DMAFired = false;


    if(activeStatus == MDA_READ_HEADER)
    {
        int32_t transferA = dma_channel_hw_addr(track1DMA)->transfer_count;
        dma_channel_abort(transferA);
        int32_t transferB = dma_channel_hw_addr(track2DMA)->transfer_count;
        dma_channel_abort(transferB);
        int32_t final = transferA - transferB;
    }

    //Abort the channels, configure as disabled and clear any pending interrupts
    dma_channel_set_irq0_enabled(track1DMA, false);
    dma_channel_abort(track1DMA);
    dma_channel_acknowledge_irq0(track1DMA);
    dma_channel_configure(track1DMA, &track1DisabledConfig, NULL, NULL, 0, false);
    
    dma_channel_set_irq1_enabled(track2DMA, false);
    dma_channel_abort(track2DMA);
    dma_channel_acknowledge_irq1(track2DMA);
    dma_channel_configure(track2DMA, &track2DisabledConfig, NULL, NULL, 0, false);
    
    //Remove the DMA IRQ handlers
    if(!readDMAs)
    /*
    {
        irq_remove_handler(DMA_IRQ_0, track1_read_irq);
        irq_remove_handler(DMA_IRQ_1, track2_read_irq);
    }
    else*/
    {
        irq_remove_handler(DMA_IRQ_0, track1_write_irq);
        irq_remove_handler(DMA_IRQ_1, track2_write_irq);
    }
}

void reset_transfer_machine(PIO pio, uint sm, uint initial_pc)
{
    // Stop machine
    pio_sm_set_enabled(pio, sm, false);

    // Clear fifos
    pio_sm_clear_fifos(pio, sm);

    // Clear FIFO debug flags
    const uint32_t fdebug_sm_mask =
            (1u << PIO_FDEBUG_TXOVER_LSB) |
            (1u << PIO_FDEBUG_RXUNDER_LSB) |
            (1u << PIO_FDEBUG_TXSTALL_LSB) |
            (1u << PIO_FDEBUG_RXSTALL_LSB);
    pio->fdebug = fdebug_sm_mask << sm;

    // Restart machine and clock
    pio_sm_restart(pio, sm);
    pio_sm_clkdiv_restart(pio, sm);

    //Clear registers and IRQs
    pio_sm_exec(pio, sm, pio_encode_mov(pio_x,pio_null));
    pio_sm_exec(pio, sm, pio_encode_mov(pio_y,pio_null));
    pio_sm_exec(pio, sm, pio_encode_irq_clear(false, 7));

    //Set PC to the start
    pio_sm_exec(pio, sm, pio_encode_jmp(initial_pc));

    //Enable the machine
    pio_sm_set_enabled(pio, sm, true);
}

//Completelly resets TX and RX state machines
void reset_transfer_machines()
{
    reset_transfer_machine(pio0, sm_read_head_1, rxProgramOffset);
    reset_transfer_machine(pio0, sm_read_head_2, rxProgramOffset);
    reset_transfer_machine(pio1, sm_write_head_1, txProgramOffset);
    reset_transfer_machine(pio1, sm_write_head_2, txProgramOffset);
}

//Initialize the six PIO state machines, two read ones, two write ones, a shifter/select one and a status one
void init_PIO_machines()
{
    //RX + Status in PIO0
    //TX + shifter in PIO1

    //Configure all the IO pins for the PIO0
    pio_gpio_init(pio0, MD_READ_HEAD_1);
    pio_gpio_init(pio0, MD_READ_HEAD_2);
    pio_gpio_init(pio0, MD_RW);
    pio_gpio_init(pio0, MD_ERASE);

    //ERASE pin needs to have a weak pull-up
    gpio_pull_up(MD_ERASE);

    gpio_pull_down(MD_READ_HEAD_1);
    gpio_pull_down(MD_READ_HEAD_2);

    //Configure all the IO pins for the PIO1
    pio_gpio_init(pio1, MD_WRITE_HEAD_1);
    pio_gpio_init(pio1, MD_WRITE_HEAD_2);
    pio_gpio_init(pio1, MD_SER_DATA_IN);
    pio_gpio_init(pio1, MD_SER_CLK);
    pio_gpio_init(pio1, MD_SER_DATA_OUT);

    gpio_pull_down(MD_WRITE_HEAD_1);
    gpio_pull_down(MD_WRITE_HEAD_2);

    //Should we add a weak pull-down to the write headers so these pull the data line low
    //on the write gap? May that disturb the read lines? Needs to be checked

    //Write and read pins are tied together for each header, as the RX pins will never become outputs it's
    //safe to do, no possibility of a shortcircuit

    /*
        Reserve one state machine to execute IRQs
    */

    sm_exec_read = pio_claim_unused_sm(pio0, true);
    pio_sm_exec(pio0, sm_exec_read, pio_encode_irq_set(false, 7));

    /*
        Reserve one state machine to execute IRQs
    */
    sm_exec_write = pio_claim_unused_sm(pio1, true);
    pio_sm_exec(pio1, sm_exec_write, pio_encode_irq_set(false, 7));

    bool irq3en = pio_interrupt_get(pio0, 7);
    bool irq4en = pio_interrupt_get(pio1, 7);
    
    if(!irq3en || !irq4en)
        return;

    /*
        Init rx machines
    */

    //Load the program
    rxProgramOffset = pio_add_program(pio0, &microdrive_read_program);

    //reserve two SM's
    sm_read_head_1 = pio_claim_unused_sm(pio0, true);
    sm_read_head_2 = pio_claim_unused_sm(pio0, true);

    //Create a generic config common to both rx machines
    pio_sm_config rx_cfg = microdrive_read_program_get_default_config(rxProgramOffset);
    sm_config_set_clkdiv(&rx_cfg, (200000000 / 100000) / 20); //we run 20 times faster than the ULA

    //Now the specific config for first state machine
    sm_config_set_set_pins(&rx_cfg, MD_READ_HEAD_1, 1);
    sm_config_set_in_pins(&rx_cfg, MD_READ_HEAD_1);
    sm_config_set_jmp_pin(&rx_cfg, MD_READ_HEAD_1);
    sm_config_set_in_shift(&rx_cfg, false, false, 0);

    //Initialize the state machine
    pio_sm_init(pio0, sm_read_head_1, rxProgramOffset, &rx_cfg);

    //start it
    pio_sm_set_enabled(pio0, sm_read_head_1, true);

    //Now go for second read state machine
    sm_config_set_set_pins(&rx_cfg, MD_READ_HEAD_2, 1);
    sm_config_set_in_pins(&rx_cfg, MD_READ_HEAD_2);
    sm_config_set_jmp_pin(&rx_cfg, MD_READ_HEAD_2);
    sm_config_set_in_shift(&rx_cfg, false, false, 0);

    //Initialize the state machine
    pio_sm_init(pio0, sm_read_head_2, rxProgramOffset, &rx_cfg);

    //start it
    pio_sm_set_enabled(pio0, sm_read_head_2, true);

    /*
        Init status machine
    */

    //Load the program
    statProgramOffset = pio_add_program(pio0, &microdrive_status_program);

    //Reserve one state machine
    sm_status = pio_claim_unused_sm(pio0, true);

    //Create config
    pio_sm_config stat_cfg = microdrive_status_program_get_default_config(statProgramOffset);
    sm_config_set_clkdiv(&stat_cfg, 1); //we run at maximum speed
    sm_config_set_set_pins(&stat_cfg, MD_RW, 2); //two pins as set, RW and ERASE
    sm_config_set_in_pins(&stat_cfg, MD_RW); //in pins start at RW
    sm_config_set_in_shift(&stat_cfg, false, false, 0);
    
    //Initialize the state machine
    pio_sm_init(pio0, sm_status, statProgramOffset, &stat_cfg);

    //start it
    pio_sm_set_enabled(pio0, sm_status, true);

    /*
        Init tx machines
    */

    //Load the program
    txProgramOffset = pio_add_program(pio1, &microdrive_write_program);

    //reserve two SM's
    sm_write_head_1 = pio_claim_unused_sm(pio1, true);
    sm_write_head_2 = pio_claim_unused_sm(pio1, true);

    //Create a generic config common to both tx machines
    pio_sm_config tx_cfg = microdrive_write_program_get_default_config(txProgramOffset);
    sm_config_set_clkdiv(&tx_cfg, 125); //we run 16 times faster than the ULA

    //Now the specific config for first state machine
    sm_config_set_set_pins(&tx_cfg, MD_WRITE_HEAD_1, 1);
    sm_config_set_sideset_pins(&tx_cfg, MD_WRITE_HEAD_1);
    sm_config_set_out_shift(&tx_cfg, true, true, 1);

    //Initialize the state machine
    pio_sm_init(pio1, sm_write_head_1, txProgramOffset, &tx_cfg);

    //start it
    pio_sm_set_enabled(pio1, sm_write_head_1, true);

    //Now go for second write state machine
    sm_config_set_set_pins(&tx_cfg, MD_WRITE_HEAD_2, 1);
    sm_config_set_sideset_pins(&tx_cfg, MD_WRITE_HEAD_2);
    sm_config_set_out_shift(&tx_cfg, true, true, 1);

    //Initialize the state machine
    pio_sm_init(pio1, sm_write_head_2, txProgramOffset, &tx_cfg);

    //start it
    pio_sm_set_enabled(pio1, sm_write_head_2, true);

    /*
        Init shifter machine
    */
    
    //Load the program
    shiftProgramOffset = pio_add_program(pio1, &microdrive_shift_select_program);

    //Reserve one state machine
    sm_shifter = pio_claim_unused_sm(pio1, true);

    //Create config
    pio_sm_config shift_cfg = microdrive_shift_select_program_get_default_config(shiftProgramOffset);
    sm_config_set_clkdiv(&shift_cfg, (200000000 / 21500) / 32); //we run 32 times faster than the serial clock (21.5Khz)
    sm_config_set_set_pins(&shift_cfg, MD_SER_DATA_IN, 3); //three pins as set, DATA_IN, CK and DATA_OUT
    sm_config_set_in_pins(&shift_cfg, MD_SER_DATA_IN); //in pins start at DATA_IN
    sm_config_set_out_pins(&shift_cfg, MD_SER_DATA_OUT, 1); //out pins start at DATA_OUT

    sm_config_set_in_shift(&shift_cfg, false, false, 0);
    sm_config_set_out_shift(&shift_cfg, true, false, 0);
    //Initialize the state machine
    pio_sm_init(pio1, sm_shifter, shiftProgramOffset, &shift_cfg);

    //start it
    pio_sm_set_enabled(pio1, sm_shifter, true);

    /*
        Configure the status program interrupt handler
    */
    pio_set_irq0_source_enabled(pio0, pis_interrupt0, true); //Enable interrupt of the SM2
    irq_set_exclusive_handler(PIO0_IRQ_0, status_irq);
    irq_set_enabled(PIO0_IRQ_0, true);

    /*
        Configure the shifter program interrupt handler
    */
    pio_set_irq0_source_enabled(pio1, pis_interrupt0, true); //Enable interrupt of the SM2
    irq_set_exclusive_handler(PIO1_IRQ_0, shifter_irq);
    irq_set_enabled(PIO1_IRQ_0, true);
}

//Initializa general GPIOs
void init_GPIO()
{
    //Initialize as output. HIGH (ULA to MD)
    gpio_init(MD_HEAD_DIR);
    gpio_set_dir(MD_HEAD_DIR, GPIO_OUT);
    gpio_put(MD_HEAD_DIR, 1);
}

//Initialize alarms for the shifter and the write gap
void init_alarms()
{
    hardware_alarm_claim(SHIFTER_ALARM);
    hardware_alarm_claim(WRITE_GAP_ALARM);

    hardware_alarm_set_callback(SHIFTER_ALARM, shifter_alarm);
    hardware_alarm_set_callback(WRITE_GAP_ALARM, write_gap_alarm);
}

//Microdrive control routine, this is the core1 main loop
void RunMDControl()
{
    //Init event machines
    event_machine_init(&mdEventQueue, &process_md_event, sizeof(mdcontrolevent_t), 16);
    event_machine_init(&uiToMdEventQueue, &process_ui_event, sizeof(utmevent_t), 8);

    //Init GPIO for dir control
    init_GPIO();
    //Init the MD PIO machines
    init_PIO_machines();
    //Initialize alarms
    init_alarms();
    //Initialize DMA channels
    init_DMAs();

    //Declare event buffers
    mdcontrolevent_t mdevtBuffer;
    utmevent_t utmevtBuffer;

    start_PIO_shifter();

    PIO tp = pio1;

    while(true)
    {
        //Process MD event queue, we process all the available events
        event_process_queue(&mdEventQueue, &mdevtBuffer, 16);
        //Process UI events, we process up to two events
        event_process_queue(&uiToMdEventQueue, &utmevtBuffer, 2);
    }
}