#include <string.h>
#include <stdio.h>
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "EventMachine.h"
#include "UserInterface.h"
#include "SharedBuffers.h"
#include "SharedEvents.h"
#include "ssd1306/ssd1306.h"
#include "pff/pff.h"

#define LED_ON(LED) gpio_put(LED, true)
#define LED_OFF(LED) gpio_put(LED, false)
#define IS_UI_DISCONNECTED() gpio_get(PIN_UI_DETECT)
#define IN_FOLDER (fno.fattrib & AM_DIR)
#define BUTTON_PRESSED(BUTTON) (!gpio_get(BUTTON))

#define PRINT_STR(STR, COL, ROW) ssd1306_draw_string(&disp, COL * 5, ROW * 8, 1, STR)
#define RENDER_SCREEN() ssd1306_show(&disp)
#define CLEAR_SCREEN() ssd1306_clear(&disp)
#define CONCAT(DEST, SOURCE) sprintf(&DEST[strlen(DEST)],"/%s", SOURCE)
USER_INTERFACE_STATE uiState = IDLE;

ssd1306_t disp;
FATFS fatfs;
DIR dir;
FILINFO fno;
char currentPath[PATH_BUFFER_SIZE];
char lineBuffer[12];
uint8_t currentSector = 0;

bool mdInUse = false;
bool firstFolderEntry = false;

CARTRIDGE_FORMAT cfInserted = NONE;

uint64_t delayEnd;
USER_INTERFACE_STATE uiNextState;

//Writes a pair of buffers of a buffer set (a buffer set are four buffers, two header ones and two sector ones)
void write_buffer_set_pair(uint8_t* source, uint8_t* track1Buffer, uint8_t* track2Buffer, bool isHeader)
{
    track2Buffer += 4; //we skip four bits on buffer 2 to respect the skewing done by the ULA

    for(int buc = 0; buc < PREAMBLE_ZERO_BITS; buc++)
    {
        track1Buffer[buc] = 0;
        track2Buffer[buc] = 0;
    } 

    track1Buffer += PREAMBLE_ZERO_BITS;
    track2Buffer += PREAMBLE_ZERO_BITS;

    for(int buc = 0; buc < PREAMBLE_ONE_BITS; buc++)
    {
        track1Buffer[buc] = 1;
        track2Buffer[buc] = 1;
    } 

    track1Buffer += PREAMBLE_ONE_BITS;
    track2Buffer += PREAMBLE_ONE_BITS;

    uint16_t copySize = isHeader ? HEADER_TRACK_DATA_SIZE : SECTOR_TRACK_DATA_SIZE;

    for(int buc = 0; buc < copySize; buc++)
    {
        uint8_t t1b = *source;
        source++;
        uint8_t t2b = *source;
        source++;

        for(int buc = 0; buc < 8; buc++)
        {
            *track1Buffer = (t1b >> buc) & 1;
            *track2Buffer = (t2b >> buc) & 1;
            track1Buffer++;
            track2Buffer++;
        }
    }
}

//Writes a buffer set with cartridge data
void write_buffer_set(uint8_t setNumber, uint8_t sector)
{
    if(setNumber == 0)
    {
        write_buffer_set_pair(&cartridge_image[CARTRIDGE_SECTOR_SIZE * sector], header_1_track_1, header_1_track_2, true);
        write_buffer_set_pair(&cartridge_image[CARTRIDGE_SECTOR_SIZE * sector + CARTRIDGE_HEADER_SIZE], sector_1_track_1, sector_1_track_2, false);
        bufferset_1_sector_number = sector;
    }
    else
    {
        write_buffer_set_pair(&cartridge_image[CARTRIDGE_SECTOR_SIZE * sector], header_2_track_1, header_2_track_2, true);
        write_buffer_set_pair(&cartridge_image[CARTRIDGE_SECTOR_SIZE * sector + CARTRIDGE_HEADER_SIZE], sector_2_track_1, sector_2_track_2, false);
        bufferset_2_sector_number = sector;
    }
}

//Find the end of a preamble from a track buffer
int8_t find_preamble_end(uint8_t* buffer)
{
    uint8_t zeroCount = 0;
    uint8_t oneCount = 0;

    //We need to find at least 16 zeros followed by 8 ones (0x00, 0x00, 0xFF)
    for(int buc = 0; buc < 100; buc++)
    {
        if(buffer[buc] == 0)
        {
            if(oneCount != 0) //Do we came here form a one?
            {
                //Reset everything
                zeroCount = 1;
                oneCount = 0;
            }
            else
                zeroCount++; //Increment count

        }
        else
        {
            if(zeroCount < 16) //Did we found a one before having eight zeros?
            {
                //Reset everything
                oneCount = 0;
                zeroCount = 0;
            }
            else
                oneCount++; //Increment count
        }

        //Have we found the eight ones?
        if(oneCount == 8)
            return buc + 1;
    }

    //Error! We haven't found the gap end!!
    return -1;

}

bool inFormat = false;
int skip = 0;

//Reads a pair of buffers from a buffer set (a buffer set are four buffers, two header ones and two sector ones)
void read_buffer_set_pair(uint8_t* destination, uint8_t* track1Buffer, uint8_t* track2Buffer, bool isHeader)
{
    uint16_t track1Pos = find_preamble_end(track1Buffer);
    uint16_t track2Pos = find_preamble_end(track2Buffer);

    uint16_t size = isHeader ? HEADER_TRACK_DATA_SIZE : SECTOR_TRACK_DATA_SIZE;

    //Check if we're writting sector 255, if true then this is a format
    if(isHeader && !inFormat)
    {
        uint8_t sectorNumber = track2Buffer[track2Pos] |
            track2Buffer[track2Pos + 1] << 1 |
            track2Buffer[track2Pos + 2] << 2 |
            track2Buffer[track2Pos + 3] << 3 |
            track2Buffer[track2Pos + 4] << 4 |
            track2Buffer[track2Pos + 5] << 5 |
            track2Buffer[track2Pos + 6] << 6 |
            track2Buffer[track2Pos + 7] << 7;

        if(sectorNumber == 255)
        {
            inFormat = 1;
            skip = currentSector;
        }
    }

    for(uint16_t buc = 0; buc < size; buc++)
    {
        *destination = track1Buffer[track1Pos] |
            track1Buffer[track1Pos + 1] << 1 |
            track1Buffer[track1Pos + 2] << 2 |
            track1Buffer[track1Pos + 3] << 3 |
            track1Buffer[track1Pos + 4] << 4 |
            track1Buffer[track1Pos + 5] << 5 |
            track1Buffer[track1Pos + 6] << 6 |
            track1Buffer[track1Pos + 7] << 7;

        track1Pos += 8;
        destination++;

        *destination = track2Buffer[track2Pos] |
            track2Buffer[track2Pos + 1] << 1 |
            track2Buffer[track2Pos + 2] << 2 |
            track2Buffer[track2Pos + 3] << 3 |
            track2Buffer[track2Pos + 4] << 4 |
            track2Buffer[track2Pos + 5] << 5 |
            track2Buffer[track2Pos + 6] << 6 |
            track2Buffer[track2Pos + 7] << 7;

        track2Pos += 8;
        destination++;

    }
}

//Reads a buffer set to the cartridge buffer
void read_buffer_set(uint8_t setNumber)
{
    if(setNumber == 0)
    {
        read_buffer_set_pair(&cartridge_image[CARTRIDGE_SECTOR_SIZE * bufferset_1_sector_number], header_1_track_1, header_1_track_2, true);
        read_buffer_set_pair(&cartridge_image[CARTRIDGE_SECTOR_SIZE * bufferset_1_sector_number + CARTRIDGE_HEADER_SIZE], sector_1_track_1, sector_1_track_2, false);
    }
    else
    {
        read_buffer_set_pair(&cartridge_image[CARTRIDGE_SECTOR_SIZE * bufferset_2_sector_number], header_2_track_1, header_2_track_2, true);
        read_buffer_set_pair(&cartridge_image[CARTRIDGE_SECTOR_SIZE * bufferset_2_sector_number + CARTRIDGE_HEADER_SIZE], sector_2_track_1, sector_2_track_2, false);
    }
}

//Process when a buffer set has been read by the ULA
void process_md_read(uint8_t bufferSet)
{
    uint8_t secNum = cartridge_image[CARTRIDGE_SECTOR_SIZE * currentSector + 1];

    //If we are in the middle of a format and we're going to send sector 254, skip it to make Minerva happy...
    if(inFormat && secNum == 254)
    {
        currentSector++;

        if(currentSector > 253)
            currentSector = 0;
    }

    write_buffer_set(bufferSet, currentSector);

    //If we are in the middle of a format and we're going to send sector 13, damage it to make Minerva happy...
    if(inFormat && secNum == 13)
    {
        cartridge_image[CARTRIDGE_SECTOR_SIZE * currentSector + 13] += 13;
        cartridge_image[CARTRIDGE_SECTOR_SIZE * currentSector + 128] += 13;
    }

    currentSector++;

    if(currentSector == 255)
        currentSector = 0;
}

//Process when a buffer set has been written by the ULA
void process_md_write(uint8_t bufferSet)
{
    read_buffer_set(bufferSet);

    uint8_t secNum = cartridge_image[CARTRIDGE_SECTOR_SIZE * currentSector + 1];

    //If we are in the middle of a format and we're going to send sector 254, skip it to make Minerva happy...
    if(inFormat && secNum == 254)
    {
        currentSector++;

        if(currentSector > 253)
            currentSector = 0;
    }

    write_buffer_set(bufferSet, currentSector);

    //If we are in the middle of a format and we're going to send sector 13, damage it to make Minerva happy...
    if(inFormat && secNum == 13)
    {
        cartridge_image[CARTRIDGE_SECTOR_SIZE * currentSector + 13] += 13;
        cartridge_image[CARTRIDGE_SECTOR_SIZE * currentSector + 128] += 13;
    }

    currentSector++;
    if(currentSector == 255)
        currentSector = 0;
}

//Process events from the MD control
void process_md_to_ui_event(void* event)
{
    mtuevent_t* evt = (mtuevent_t*)event;

    switch(evt->event)
    {
        case MTU_MD_DESELECTED:

            mdInUse = false;
            inFormat = false;
            LED_OFF(PIN_LED_SELECT);
            LED_OFF(PIN_LED_READ);
            LED_OFF(PIN_LED_WRITE);
            break;

        case MTU_MD_SELECTED:

            mdInUse = true;
            LED_ON(PIN_LED_SELECT);
            break;

        case MTU_MD_READING:
            LED_ON(PIN_LED_READ);
            LED_OFF(PIN_LED_WRITE);
            break;

        case MTU_MD_WRITTING:
            LED_ON(PIN_LED_WRITE);
            LED_OFF(PIN_LED_READ);
            break;

        case MTU_BUFFERSET_READ:

            process_md_read(evt->arg);
            break;

        case MTU_BUFFERSET_WRITTEN:

            process_md_write(evt->arg);
            break;
    }
}

//Initialize the I2C screen
bool init_screen()
{
    disp.external_vcc=false;
    
    if(!ssd1306_init(&disp, 64, 32, 0x3C, I2C_PORT))
        return false;

    CLEAR_SCREEN();
    RENDER_SCREEN();

    return true;
}

//Show the current file name to the screen
void show_file_name()
{
    CLEAR_SCREEN();
    PRINT_STR(IN_FOLDER ? "> Folder" : "> File", 0, 0);

    if(strlen(fno.fname) > 11)
    {
        memset(lineBuffer, 0, 12);
        memcpy(lineBuffer, fno.fname, 8);
        PRINT_STR(lineBuffer, 0, 2);
        sprintf(lineBuffer, "       %s", &fno.fname[8]);
        PRINT_STR(lineBuffer, 0, 3);
    }
    else
        PRINT_STR(fno.fname, 0, 2);

    RENDER_SCREEN();
}

//Remove from the path buffer the last entry
void rewind_path()
{
    if(strlen(currentPath) == 0)
        return;

    char* lastPos = strrchr(currentPath, '/');

    if(lastPos == NULL)
        return;

    memset(lastPos, 0, (size_t)(PATH_BUFFER_SIZE - (lastPos - currentPath)));
}

//Debounce a button press
void debounce_button(uint button)
{
    while(BUTTON_PRESSED(button))
    {
        sleep_ms(20);
    }
    sleep_ms(200);
}

void fix_cartridge_checksums()
{
    SECTOR_t* sector = (SECTOR_t*)cartridge_image;

    for(int buc = 0; buc < 255; buc++)
    {
        uint16_t computedChecksum = 0;

        for(int hBuc = 0; hBuc < 14; hBuc++)
            computedChecksum += sector->Header.HeaderData[hBuc];

        computedChecksum += 0x0f0f;
        sector->Header.Checksum = computedChecksum;

        computedChecksum = 0;

        for(int hrBuc = 0; hrBuc < 2; hrBuc++)
            computedChecksum += sector->Record.HeaderData[hrBuc];

        computedChecksum += 0x0f0f;
        sector->Record.HeaderChecksum = computedChecksum;

        computedChecksum = 0;

        for(int hdBuc = 0; hdBuc < 512; hdBuc++)
            computedChecksum += sector->Record.Data[hdBuc];

        computedChecksum += 0x0f0f;
        sector->Record.DataChecksum = computedChecksum;

        for (int bExtra = 0; bExtra < 84; bExtra++)
                sector->Record.ExtraBytes[bExtra] = bExtra % 2 == 0 ? 0xAA : 0x55;

        if (sector->Record.ExtraBytesChecksum != 0x3b19)
            sector->Record.ExtraBytesChecksum = 0x3b19;

        sector++;
    }
}

//Save the cartridge to a mdv image
bool save_mdv_cartridge()
{
    if(pf_open(currentPath))
        return false;

    UINT writeSize;
    uint8_t tmpByte = 0;
    int bufferPos = 0;

    uint8_t padBuffer[MDV_PAD_SIZE];
    memset(padBuffer, 'Z', MDV_PAD_SIZE);

    for(int buc = 0; buc < 255; buc++)
    {
        tmpByte = 0;

        for(int zeros = 0; zeros < PREAMBLE_ZERO_BYTES; zeros++)
        {
            if(pf_write(&tmpByte, 1, &writeSize))
            {
                pf_write(0, 0, &writeSize);
                return false;
            }

            if(writeSize != 1)
            {
                pf_write(0, 0, &writeSize);
                return false;
            }
        }

        tmpByte = 0xff;

        for(int ones = 0; ones < PREAMBLE_ONE_BYTES; ones++)
        {
            if(pf_write(&tmpByte, 1, &writeSize))
            {
                pf_write(0, 0, &writeSize);
                return false;
            }

            if(writeSize != 1)
            {
                pf_write(0, 0, &writeSize);
                return false;
            }
        }

        if(pf_write(&cartridge_image[bufferPos], CARTRIDGE_HEADER_SIZE, &writeSize))
        {
            pf_write(0, 0, &writeSize);
            return false;
        }

        if(writeSize != CARTRIDGE_HEADER_SIZE)
        {
            pf_write(0, 0, &writeSize);
            return false;
        }

        bufferPos += CARTRIDGE_HEADER_SIZE;

        tmpByte = 0;

        for(int zeros = 0; zeros < PREAMBLE_ZERO_BYTES; zeros++)
        {
            if(pf_write(&tmpByte, 1, &writeSize))
            {
                pf_write(0, 0, &writeSize);
                return false;
            }

            if(writeSize != 1)
            {
                pf_write(0, 0, &writeSize);
                return false;
            }
        }

        tmpByte = 0xff;

        for(int ones = 0; ones < PREAMBLE_ONE_BYTES; ones++)
        {
            if(pf_write(&tmpByte, 1, &writeSize))
            {
                pf_write(0, 0, &writeSize);
                return false;
            }

            if(writeSize != 1)
            {
                pf_write(0, 0, &writeSize);
                return false;
            }
        }

        if(pf_write(&cartridge_image[bufferPos], CARTRIDGE_DATA_SIZE, &writeSize))
        {
            pf_write(0, 0, &writeSize);
            return false;
        }

        if(writeSize != CARTRIDGE_DATA_SIZE)
            return false;

        bufferPos += CARTRIDGE_DATA_SIZE;

        if(pf_write(padBuffer, MDV_PAD_SIZE, &writeSize))
        {
            pf_write(0, 0, &writeSize);
            return false;
        }

        if(writeSize != MDV_PAD_SIZE)
            return false;
    }

    pf_write(0, 0, &writeSize);

    return true;
}

//Save the cartridge to a mpd image
bool save_mpd_cartridge()
{
    if(pf_open(currentPath))
        return false;

    UINT writeSize;

    if(pf_write(cartridge_image, CART_SIZE, &writeSize))
    {
        pf_write(0, 0, &writeSize);
        return false;
    }

    if(writeSize != CART_SIZE)
    {
        pf_write(0, 0, &writeSize);
        return false;
    }

    pf_write(0, 0, &writeSize);

    return true;
}

//Load a MDV image to the cartridge buffer
bool load_mdv_cartridge()
{
    if(pf_open(currentPath))
        return false;

    UINT readSize = 0;

    int bufferPos = 0;
    int filePos = 0;

    for(int buc = 0; buc < 255; buc++)
    {
        filePos = buc * MDV_SECTOR_SIZE + MDV_PREAMBLE_SIZE; //skip preamble

        if(pf_lseek(filePos))
            return false;

        if(pf_read(&cartridge_image[bufferPos], MDV_HEADER_SIZE, &readSize))
            return false;

        if(readSize != MDV_HEADER_SIZE)
            return false;

        filePos += MDV_HEADER_SIZE + MDV_PREAMBLE_SIZE;
        bufferPos += MPD_HEADER_SIZE;

        if(pf_lseek(filePos))
            return false;

        if(pf_read(&cartridge_image[bufferPos], MPD_DATA_SIZE, &readSize))
            return false;

        if(readSize != MPD_DATA_SIZE)
            return false;

        bufferPos += MPD_DATA_SIZE;
    }

    return true;
}

//Load a MPD image to the cartridge buffer
bool load_mpd_cartridge()
{
    if(pf_open(currentPath))
        return false;

    UINT readSize = 0;

    if(pf_read(cartridge_image, CART_MPD_SIZE, &readSize))
        return false;

    if(readSize != CART_MPD_SIZE)
        return false;

    return true;

}

//Check if cancel was requested
void check_cancel()
{
    if(BUTTON_PRESSED(PIN_BTN_BACK))
    {
        debounce_button(PIN_BTN_BACK);
        rewind_path();
        uiState = OPEN_FOLDER;
        cfInserted = NONE;
        utmevent_t removeEvt;
        currentSector = 0;
        removeEvt.event = UTM_CARTRIDGE_REMOVED;
        event_push(&uiToMdEventQueue, &removeEvt);
    }
}

void program_delay(uint64_t ms_delay, USER_INTERFACE_STATE nextState)
{
    delayEnd = time_us_64() + (ms_delay * 1000);
    uiNextState = nextState;
    uiState = DELAY;
}

void check_delay()
{
    uint64_t currentTime = time_us_64();

    if(currentTime >= delayEnd)
        uiState = uiNextState;
}

//Process the user interface state machine
void process_user_interface()
{
    switch(uiState)
    {
        case IDLE:

            if(!IS_UI_DISCONNECTED())
                program_delay(2000, INIT_SCREEN);
            
            break;

        case DELAY:

            if(IS_UI_DISCONNECTED())
                uiState = IDLE;
            else
                check_delay();

            break;

        case INIT_SCREEN:

            if(IS_UI_DISCONNECTED())
                uiState = IDLE;
            else
            {
                if(init_screen())
                    uiState = WELCOME;
            }
            break;

        case WELCOME:

            if(IS_UI_DISCONNECTED())
                uiState = IDLE;
            else
            {
                CLEAR_SCREEN();
                PRINT_STR(" MicroPico ", 0, 1);
                PRINT_STR("   Drive   ", 0, 2);
                PRINT_STR("    1.0    ", 0, 3);
                RENDER_SCREEN();
                
                if(cfInserted == NONE)
                    memset(currentPath, 0, PATH_BUFFER_SIZE);

                program_delay(2000, SHOW_WAITING_SD_CARD);
            }
            break;

        case SHOW_WAITING_SD_CARD:

            if(IS_UI_DISCONNECTED())
                uiState = IDLE;
            else
            {
                CLEAR_SCREEN();
                PRINT_STR("Waiting SD ", 0, 1);
                PRINT_STR("card...    ", 0, 2);
                RENDER_SCREEN();
                uiState = WAITING_SD_CARD;
            }
            break;

        case WAITING_SD_CARD:

            if(IS_UI_DISCONNECTED())
                uiState = IDLE;
            else
            {
                if(!pf_mount(&fatfs))
                {
                    if(cfInserted == NONE)
                        uiState = OPEN_FOLDER;
                    else
                    {
                        uiState = CARTRIDGE_READY;
                        CLEAR_SCREEN();
                        PRINT_STR("Cartridge  ", 0, 1);
                        PRINT_STR("ready.     ", 0, 2);
                        RENDER_SCREEN();
                    }
                }
            }

            break;

        case OPEN_FOLDER:

            if(IS_UI_DISCONNECTED())
                uiState = IDLE;
            else
            {
                if(pf_opendir(&dir, currentPath))
                {
                    CLEAR_SCREEN();
                    PRINT_STR("Error      ", 0, 1);
                    PRINT_STR("opening    ", 0, 2);
                    PRINT_STR("folder.    ", 0, 3);
                    RENDER_SCREEN();
                    sleep_ms(2000);
                    uiState = WAITING_SD_CARD;
                }
                else
                {
                    firstFolderEntry = true;
                    uiState = READ_FOLDER_ENTRY;
                }
            }

            break;

        case READ_FOLDER_ENTRY:

            if(IS_UI_DISCONNECTED())
                uiState = IDLE;
            else
            {
                if(pf_readdir(&dir, &fno))
                {
                    CLEAR_SCREEN();
                    PRINT_STR("Error      ", 0, 1);
                    PRINT_STR("reading    ", 0, 2);
                    PRINT_STR("folder.    ", 0, 3);
                    RENDER_SCREEN();
                    sleep_ms(2000);
                    uiState = WAITING_SD_CARD;
                }
                else
                {
                    if(fno.fname[0] == 0)
                    {
                        if(firstFolderEntry)
                        {
                            CLEAR_SCREEN();
                            PRINT_STR("Empty      ", 0, 1);
                            PRINT_STR("folder.    ", 0, 2);
                            RENDER_SCREEN();
                        }
                        else
                        {
                            uiState = OPEN_FOLDER;
                        }
                    }
                    else
                    {
                        firstFolderEntry = false;
                        show_file_name();
                        uiState = SELECT_FILE;
                    }
                }
            }

            break;

        case SELECT_FILE:

            if(IS_UI_DISCONNECTED())
                uiState = IDLE;
            else
            {
                if(BUTTON_PRESSED(PIN_BTN_SELECT) && !firstFolderEntry)
                {
                    debounce_button(PIN_BTN_SELECT);

                    if(IN_FOLDER)
                    {
                        CONCAT(currentPath, fno.fname);
                        uiState = OPEN_FOLDER;
                    }
                    else
                        uiState = FILE_SELECTED;
                }
                else if(BUTTON_PRESSED(PIN_BTN_NEXT) && !firstFolderEntry)
                {
                    debounce_button(PIN_BTN_NEXT);
                    uiState = READ_FOLDER_ENTRY;
                }
                else if(BUTTON_PRESSED(PIN_BTN_BACK))
                {
                    debounce_button(PIN_BTN_BACK);
                    rewind_path();
                    uiState = OPEN_FOLDER;
                }
            }

            break;

        case FILE_SELECTED:

            if(IS_UI_DISCONNECTED())
                uiState = IDLE;
            else
            {
                CLEAR_SCREEN();

                switch(fno.fsize)
                {

                    case CART_MDV_SIZE:

                        PRINT_STR("Loading MDV", 0, 1);
                        PRINT_STR("cartridge..", 0, 2);
                        RENDER_SCREEN();
                        cfInserted = MDV;
                        uiState = FILE_LOAD;
                        break;

                    case CART_MPD_SIZE:

                        PRINT_STR("Loading MPD", 0, 1);
                        PRINT_STR("cartridge..", 0, 2);
                        RENDER_SCREEN();
                        cfInserted = MPD;
                        uiState = FILE_LOAD;
                        break;

                    default:

                        PRINT_STR("Unknown", 0, 1);
                        PRINT_STR("cartridge", 0, 2);
                        PRINT_STR("format.", 0, 3);
                        RENDER_SCREEN();
                        sleep_ms(4000);
                        show_file_name();
                        uiState = SELECT_FILE;
                        break;

                }
            }

            break;

        case FILE_LOAD:

            if(IS_UI_DISCONNECTED())
                uiState = IDLE;
            else
            {
                CONCAT(currentPath, fno.fname);

                bool res = false;
                
                switch(cfInserted)
                {
                    case MDV:
                        res = load_mdv_cartridge();
                        break;
                    case MPD:
                        res = load_mpd_cartridge();
                        break;
                }
                
                if(!res)
                {
                    CLEAR_SCREEN();
                    PRINT_STR("Error", 0, 1);
                    PRINT_STR("loading", 0, 2);
                    PRINT_STR("cartridge.", 0, 3);
                    RENDER_SCREEN();
                    rewind_path();
                    sleep_ms(4000);
                    show_file_name();
                    cfInserted = NONE;
                    uiState = SELECT_FILE;
                }
                else
                {

                    CLEAR_SCREEN();
                    PRINT_STR("Validating", 0, 1);
                    PRINT_STR("cartridge", 0, 2);
                    PRINT_STR("format...", 0, 3);
                    RENDER_SCREEN();

                    fix_cartridge_checksums();

                    write_buffer_set(0, 0);
                    write_buffer_set(1, 1);
                    currentSector = 2;
                    uiState = CARTRIDGE_READY;
                    utmevent_t insertEvt;
                    insertEvt.event = UTM_CARTRIDGE_INSERTED;
                    event_push(&uiToMdEventQueue, &insertEvt);
                    CLEAR_SCREEN();
                    PRINT_STR("Cartridge  ", 0, 1);
                    PRINT_STR("ready.     ", 0, 2);
                    RENDER_SCREEN();
                }
            }

            break;

        case CARTRIDGE_READY:

            if(IS_UI_DISCONNECTED())
                uiState = IDLE;
            else
            {
                if(BUTTON_PRESSED(PIN_BTN_BACK))
                {
                    debounce_button(PIN_BTN_BACK);
                    rewind_path();
                    uiState = OPEN_FOLDER;
                    cfInserted = NONE;
                    utmevent_t removeEvt;
                    removeEvt.event = UTM_CARTRIDGE_REMOVED;
                    event_push(&uiToMdEventQueue, &removeEvt);
                }
                else if(BUTTON_PRESSED(PIN_BTN_SELECT))
                {
                    CLEAR_SCREEN();
                    PRINT_STR("Saving     ", 0, 1);
                    PRINT_STR("cartridge..", 0, 2);
                    RENDER_SCREEN();

                    bool res = false;

                    switch(cfInserted)
                    {
                        case MDV:
                            res = save_mdv_cartridge();
                            break;
                        //case IMG:
                        //    res = save_img_cartridge();
                        //    break;
                        case MPD:
                            res = save_mpd_cartridge();
                            break;
                    }

                    if(res)
                    {
                        CLEAR_SCREEN();
                        PRINT_STR("Cartridge  ", 0, 1);
                        PRINT_STR("saved.     ", 0, 2);
                        RENDER_SCREEN();
                        sleep_ms(2000);
                        CLEAR_SCREEN();
                        PRINT_STR("Cartridge  ", 0, 1);
                        PRINT_STR("ready.     ", 0, 2);
                        RENDER_SCREEN();
                    }
                    else
                    {
                        CLEAR_SCREEN();
                        PRINT_STR("Error      ", 0, 1);
                        PRINT_STR("saving     ", 0, 2);
                        PRINT_STR("cartridge. ", 0, 3);
                        RENDER_SCREEN();
                        sleep_ms(2000);
                        CLEAR_SCREEN();
                        PRINT_STR("Cartridge  ", 0, 1);
                        PRINT_STR("ready.     ", 0, 2);
                        RENDER_SCREEN();
                    }
                }
            }

            break;

    }
}

//Initialize UI leds
void init_leds()
{
    gpio_init(PIN_LED_ON);
    gpio_init(PIN_LED_SELECT);
    gpio_init(PIN_LED_READ);
    gpio_init(PIN_LED_WRITE);

    gpio_set_dir(PIN_LED_ON, true);
    gpio_set_dir(PIN_LED_SELECT, true);
    gpio_set_dir(PIN_LED_READ, true);
    gpio_set_dir(PIN_LED_WRITE, true);

    gpio_put(PIN_LED_ON, 1);
}

//Initialize UI buttons
void init_buttons()
{
    gpio_init(PIN_BTN_BACK);
    gpio_init(PIN_BTN_NEXT);
    gpio_init(PIN_BTN_SELECT);
    gpio_init(PIN_UI_DETECT);

    gpio_set_dir(PIN_BTN_BACK, false);
    gpio_set_dir(PIN_BTN_NEXT, false);
    gpio_set_dir(PIN_BTN_SELECT, false);
    gpio_set_dir(PIN_UI_DETECT, false);

    gpio_pull_up(PIN_BTN_BACK);
    gpio_pull_up(PIN_BTN_NEXT);
    gpio_pull_up(PIN_BTN_SELECT);
    gpio_pull_up(PIN_UI_DETECT);
}

//Initialize the I2C bus
void init_i2c()
{
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

//Main user interface loop
void RunUserInterface()
{
    event_machine_init(&mdToUiEventQueue, &process_md_to_ui_event, sizeof(mtuevent_t), 8);
    mtuevent_t mtuevtBuffer;

    init_leds();
    init_buttons();
    init_i2c();

    while(true)
    {
        event_process_queue(&mdToUiEventQueue, &mtuevtBuffer, 16);

        if(!mdInUse)
            process_user_interface();
        else
            check_cancel();

    }
}