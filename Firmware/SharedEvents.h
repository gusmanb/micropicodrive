#ifndef __SHAREDEVENTS__
#define __SHAREDEVENTS__

typedef enum __attribute__((packed))
{
    MTU_MD_DESELECTED,
    MTU_MD_SELECTED,
    MTU_MD_READING, //ULA is reading from the MD
    MTU_MD_WRITTING, //ULA is writting to the MD
    MTU_BUFFERSET_READ, //A buffer set (header + sector) has been read by the ULA
    MTU_BUFFERSET_WRITTEN, //A buffer set (header + sector) has been written by the ULA

} mdtouievent_t;

typedef enum __attribute__((packed))
{
    UTM_CARTRIDGE_INSERTED,
    UTM_CARTRIDGE_REMOVED

} uitomdevent_t; 

typedef struct mtuevent
{
    mdtouievent_t event;
    uint8_t arg;

} mtuevent_t;

typedef struct utmevent
{
    uitomdevent_t event;

} utmevent_t;

#endif