
#ifndef __USERINTERFACE__
#define __USERINTERFACE__

#define PIN_LED_ON 25

#define PIN_LED_SELECT 9
#define PIN_LED_READ 11
#define PIN_LED_WRITE 10

#define PIN_BTN_BACK 12
#define PIN_BTN_NEXT 13
#define PIN_BTN_SELECT 14

#define PIN_UI_DETECT 15

#define I2C_PORT i2c0
#define I2C_SDA 20
#define I2C_SCL 21

#define CART_MDV_SIZE 174930
#define CART_MPD_SIZE 160140

#define MDV_PREAMBLE_SIZE 12
#define MDV_HEADER_SIZE 16
#define MDV_DATA_SIZE 646
#define MDV_PAD_SIZE 34
#define MDV_SECTOR_SIZE 686

#define MPD_HEADER_SIZE 16
#define MPD_DATA_SIZE 612

#define CARTRIDGE_HEADER_SIZE 16
#define CARTRIDGE_DATA_SIZE 612
#define CARTRIDGE_SECTOR_SIZE 628
#define CARTRIDGE_SECTOR_COUNT 255

#define PATH_BUFFER_SIZE 300

typedef enum
{
    IDLE,
    DELAY,
    INIT_SCREEN,
    WELCOME,
    SHOW_WAITING_SD_CARD,
    WAITING_SD_CARD,
    OPEN_FOLDER,
    READ_FOLDER_ENTRY,
    SELECT_FILE,
    FILE_SELECTED,
    FILE_LOAD,
    CARTRIDGE_READY

} USER_INTERFACE_STATE;

typedef enum
{
    NONE,
    MDV,
    MPD
} CARTRIDGE_FORMAT;

typedef struct __attribute__((__packed__)) SECTOR_HEADER
{
    uint8_t HeaderData[14];
    uint16_t Checksum;

} SECTOR_HEADER_t;

typedef struct __attribute__((__packed__)) SECTOR_RECORD
{
    uint8_t HeaderData[2];
    uint16_t HeaderChecksum;
    uint8_t FilePreamble[8];
    uint8_t Data[512];
    uint16_t DataChecksum;
    uint8_t ExtraBytes[84];
    uint16_t ExtraBytesChecksum;

} SECTOR_RECORD_t;

typedef struct __attribute__((__packed__)) SECTOR
{
    SECTOR_HEADER_t Header;
    SECTOR_RECORD_t Record;

} SECTOR_t;

void RunUserInterface();

#endif