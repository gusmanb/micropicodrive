#ifndef __MICRODRIVECONTROL__
#define __MICRODRIVECONTROL__

#include "hardware/pio.h"

#define MD_SER_DATA_IN 0
#define MD_SER_CLK 1
#define MD_SER_DATA_OUT 2
#define MD_READ_HEAD_1 3
#define MD_READ_HEAD_2 5
#define MD_WRITE_HEAD_1 4
#define MD_WRITE_HEAD_2 6
#define MD_RW 7
#define MD_ERASE 8
#define MD_HEAD_DIR 22

#define QL_HEADER_SIZE 71
#define QL_SECTOR_SIZE 2455
#define QL_PREAMBLE_SIZE 48

#define SHIFTER_ALARM 0
#define WRITE_GAP_ALARM 1

//2780
#define QL_WRITE_GAP_US 3600
#define SHIFTER_SELECT_US 10000

//Enumeration with the meaning of the status lines
typedef enum __attribute__((packed))
{
    MDL_WRITE = 0, //ULA wants to write to the MD
    MDL_WRITE_GAP = 1, //Write gap 
    MDL_INVALID = 2,
    MDL_READ = 3 //ULA wants to read from the MD

} mdlinestatus_t;

//Status of the md itself
typedef enum __attribute__((packed))
{
    MDS_DESELECTED,
    MDS_SELECTED

} mdstatus_t;

//Selected substatus
typedef enum __attribute__((packed))
{
    MDA_IDLE,
    MDA_READ_HEADER_GAP,
    MDA_READ_HEADER,
    MDA_READ_SECTOR_GAP,
    MDA_READ_SECTOR,
    MDA_WRITE_HEADER_GAP,
    MDA_WRITE_HEADER,
    MDA_WRITE_SECTOR_GAP,
    MDA_WRITE_SECTOR

} mdactivestatus_t;

//Internal events for the md control
typedef enum __attribute__((packed))
{
    MDE_SHIFT_CHANGED, //Shifter status has changed (0 or 1)
    MDE_SELECT_TIMEOUT_EXPIRED, //Shifter selection time has expired, MD has been selected or deselected
    MDE_MD_STATUS_CHANGED, //Status lines have changed
    MDE_DMA_READ_IRQ, //A read IRQ has been raised
    MDE_DMA_WRITE_IRQ, //A write IRQ has been changed
    MDE_CHECK_WRITE_FINISH, //Check if the TX buffers are empty and the transfer has finished
    MDE_WRITE_GAP_FINISHED //A write gap has finished

} mdevents_t;

typedef struct mdcontrolevent
{

    mdevents_t event;
    uint16_t args;

} mdcontrolevent_t;

void select_md();
void deselect_md();
void check_ui_notifications(mdactivestatus_t previousState, bool fromGap);
bool common_gap_code(uint8_t** selectedTrack1Buffer, uint8_t** selectedTrack2Buffer, bool forRead);
void begin_write_gap();
void end_write_gap();
void begin_read_gap();
void end_read_gap();
void shifter_alarm(uint alarm_num);
void write_gap_alarm(uint alarm_num);
static inline void abort_shifter_alarm();
static inline void abort_write_gap_alarm();
static inline void begin_shifter_alarm();
static inline void begin_write_gap_alarm();
static inline void start_PIO_shifter();
static inline void deselect_PIO_status();
static inline void select_PIO_status();
static inline void begin_PIO_read_gap();
static inline void end_PIO_read_gap();
static inline void sleep_PIO_write();
static inline void begin_PIO_write_gap();
static inline void end_PIO_write_gap();
void process_md_event(void* event);
void process_ui_event(void* event);
void track1_write_irq();
void track2_write_irq();
void status_irq();
void shifter_irq();
void init_DMAs();
void enable_read_DMAs(uint8_t* buffer_track_1, uint8_t* buffer_track_2, bool isHeader);
void enable_write_DMAs(uint8_t* buffer_track_1, uint8_t* buffer_track_2, bool isHeader);
void disable_DMAs(bool readDMAs);
void reset_transfer_machine(PIO pio, uint sm, uint initial_pc);
void reset_transfer_machines();
void init_PIO_machines();
void init_alarms();
void RunMDControl();

#endif