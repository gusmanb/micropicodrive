/*-----------------------------------------------------------------------
/  PFF - Low level disk interface modlue include file    (C)ChaN, 2014
/-----------------------------------------------------------------------*/

#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

#include "pff.h"


/* Status of Disk Functions */
typedef BYTE	DSTATUS;


/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Function succeeded */
	RES_ERROR,		/* 1: Disk error */
	RES_NOTRDY,		/* 2: Not ready */
	RES_PARERR		/* 3: Invalid parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */

DSTATUS disk_initialize (void);
DRESULT disk_readp (BYTE* buff, DWORD sector, UINT offser, UINT count);
DRESULT disk_writep (const BYTE* buff, DWORD sc);

#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */


/*------------------------*/
/* Implementation defines */

#ifdef PF_USE_SPI0
#define PF_SPI spi0
#elif PF_USE_SPI1
#define PF_SPI spi1
#else
#error "SPI port not defined"
#endif

/* Definitions for MMC/SDC command */
#define CMD0 (0x40 + 0)    /* GO_IDLE_STATE */
#define CMD1 (0x40 + 1)    /* SEND_OP_COND (MMC) */
#define ACMD41 (0xC0 + 41) /* SEND_OP_COND (SDC) */
#define CMD8 (0x40 + 8)    /* SEND_IF_COND */
#define CMD16 (0x40 + 16)  /* SET_BLOCKLEN */
#define CMD17 (0x40 + 17)  /* READ_SINGLE_BLOCK */
#define CMD24 (0x40 + 24)  /* WRITE_BLOCK */
#define CMD55 (0x40 + 55)  /* APP_CMD */
#define CMD58 (0x40 + 58)  /* READ_OCR */

/* Card type flags (CardType) */
#define CT_MMC 0x01   /* MMC ver 3 */
#define CT_SD1 0x02   /* SD ver 1 */
#define CT_SD2 0x04   /* SD ver 2 */
#define CT_BLOCK 0x08 /* Block addressing */


#define CLK_SLOW	100000

#ifdef __cplusplus
}
#endif

#endif	/* _DISKIO_DEFINED */
