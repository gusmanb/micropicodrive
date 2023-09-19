/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2014      */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
/*--------------------------------------------------------------------------
   SPI and Pin selection
---------------------------------------------------------------------------*/

#define SELECT() gpio_put(PF_SPI_CS, 0)
#define DESELECT() gpio_put(PF_SPI_CS, 1)
#define SELECTING (gpio_is_dir_out(PF_SPI_CS) && !gpio_get(PF_SPI_CS))
#define FCLK_SLOW() spi_set_baudrate(spi, CLK_SLOW)
#define FCLK_FAST() spi_set_baudrate(spi, CLK_FAST)

spi_inst_t *spi = PF_SPI;

static
BYTE CardType;			/* Card type flags */

static inline uint32_t _millis(void)
{
    return to_ms_since_boot(get_absolute_time());
}

/*-----------------------------------------------------------------------*/
/* SPI controls (Platform dependent)                                     */
/*-----------------------------------------------------------------------*/

/* Initialize MMC interface */
void init_spi(void)
{
	/* GPIO pin configuration */
	/* pull up of MISO is MUST (10Kohm external pull up is recommended) */
	/* Set drive strength and slew rate if needed to meet wire condition */
	gpio_init(PF_SPI_SCK);
	gpio_disable_pulls(PF_SPI_SCK);
	//gpio_pull_up(PIN_SPI_SCK);
	//gpio_set_drive_strength(PIN_SPI_SCK, PADS_BANK0_GPIO0_DRIVE_VALUE_4MA); // 2mA, 4mA (default), 8mA, 12mA
	//gpio_set_slew_rate(PIN_SPI_SCK, 0); // 0: SLOW (default), 1: FAST
	gpio_set_function(PF_SPI_SCK, GPIO_FUNC_SPI);

	gpio_init(PF_SPI_MISO);
	#ifdef PF_MISO_PULLUP
	gpio_pull_up(PF_SPI_MISO);
	#else
	gpio_disable_pulls(PF_SPI_MISO);
	#endif
	gpio_set_function(PF_SPI_MISO, GPIO_FUNC_SPI);

	gpio_init(PF_SPI_MOSI);
	gpio_disable_pulls(PF_SPI_MOSI);
	//gpio_pull_up(PIN_SPI_MOSI);
	//gpio_set_drive_strength(PIN_SPI_MOSI, PADS_BANK0_GPIO0_DRIVE_VALUE_4MA); // 2mA, 4mA (default), 8mA, 12mA
	//gpio_set_slew_rate(PIN_SPI_MOSI, 0); // 0: SLOW (default), 1: FAST
	gpio_set_function(PF_SPI_MOSI, GPIO_FUNC_SPI);

	gpio_init(PF_SPI_CS);
	gpio_disable_pulls(PF_SPI_CS);
	//gpio_pull_up(PIN_SPI_CS);
	//gpio_set_drive_strength(PIN_SPI_CS, PADS_BANK0_GPIO0_DRIVE_VALUE_4MA); // 2mA, 4mA (default), 8mA, 12mA
	//gpio_set_slew_rate(PIN_SPI_CS, 0); // 0: SLOW (default), 1: FAST
	gpio_set_dir(PF_SPI_CS, GPIO_OUT);

	/* chip _select invalid*/
	DESELECT();

	spi_init(spi, CLK_SLOW);

	/* SPI parameter config */
	spi_set_format(spi,
		8, /* data_bits */
		SPI_CPOL_0, /* cpol */
		SPI_CPHA_0, /* cpha */
		SPI_MSB_FIRST /* order */
	);
}

/* Exchange a byte */
static inline BYTE spi_exchange (
	BYTE dat	/* Data to send */
)
{
	spi_write_read_blocking(spi, &dat, &dat, 1);
	return dat;
}

//Send a byte
static inline void xmit_spi(BYTE d)
{
	spi_exchange(d);
}

//Receive a byte
static inline BYTE rcv_spi(void)
{
	return spi_exchange(0xFF);
}

/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/
static BYTE send_cmd(BYTE  cmd, /* 1st byte (Start + Index) */
                     DWORD arg  /* Argument (32 bits) */
)
{
	BYTE n, res;

	if (cmd & 0x80) { /* ACMD<n> is the command sequence of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1)
			return res;
	}

	/* Select the card */
	DESELECT();
	rcv_spi();
	SELECT();
	rcv_spi();

	/* Send a command packet */
	xmit_spi(cmd);               /* Start + Command index */
	xmit_spi((BYTE)(arg >> 24)); /* Argument[31..24] */
	xmit_spi((BYTE)(arg >> 16)); /* Argument[23..16] */
	xmit_spi((BYTE)(arg >> 8));  /* Argument[15..8] */
	xmit_spi((BYTE)arg);         /* Argument[7..0] */
	n = 0x01;                    /* Dummy CRC + Stop */
	if (cmd == CMD0)
		n = 0x95; /* Valid CRC for CMD0(0) */
	if (cmd == CMD8)
		n = 0x87; /* Valid CRC for CMD8(0x1AA) */
	xmit_spi(n);

	/* Receive a command response */
	n = 10; /* Wait for a valid response in timeout of 10 attempts */
	do {
		res = rcv_spi();
	} while ((res & 0x80) && --n);

	return res; /* Return with the response value */
}


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (void)
{
	BYTE n, cmd, ty, ocr[4];
	UINT tmr;

#if PF_USE_WRITE
	if (CardType && SELECTING)
		disk_writep(0, 0); /* Finalize write process if it is in progress */
#endif

	init_spi(); /* Initialize ports to control MMC */
	DESELECT();
	FCLK_SLOW();
	for (n = 10; n; n--)
		rcv_spi(); /* 80 dummy clocks with CS=H */

	ty = 0;
	if (send_cmd(CMD0, 0) == 1) 
	{         /* GO_IDLE_STATE */
		if (send_cmd(CMD8, 0x1AA) == 1) 
		{ /* SDv2 */
			for (n = 0; n < 4; n++)
				ocr[n] = rcv_spi();                 /* Get trailing return value of R7 resp */

			if (ocr[2] == 0x01 && ocr[3] == 0xAA) 
			{ /* The card can work at vdd range of 2.7-3.6V */
				for (tmr = 10000; tmr && send_cmd(ACMD41, 1UL << 30); tmr--)
					sleep_us(100);                   /* Wait for leaving idle state (ACMD41 with HCS bit) */

				if (tmr && send_cmd(CMD58, 0) == 0) 
				{ /* Check CCS bit in the OCR */
					for (n = 0; n < 4; n++)
						ocr[n] = rcv_spi();

					ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2; /* SDv2 (HC or SC) */
				}
			}
		} else { /* SDv1 or MMCv3 */
			if (send_cmd(ACMD41, 0) <= 1) 
			{
				ty  = CT_SD1;
				cmd = ACMD41; /* SDv1 */
			} else 
			{
				ty  = CT_MMC;
				cmd = CMD1; /* MMCv3 */
			}

			for (tmr = 10000; tmr && send_cmd(cmd, 0); tmr--)
				sleep_us(100);                    /* Wait for leaving idle state */

			if (!tmr || send_cmd(CMD16, 512) != 0) /* Set R/W block length to 512 */
				ty = 0;
		}
	}

	CardType = ty;
	DESELECT();
	rcv_spi();

	if (ty) 
	{			/* OK */
		FCLK_FAST();			/* Set fast clock */
		return 0;
	} 
	else 
	{
		return STA_NOINIT;
	}
}



/*-----------------------------------------------------------------------*/
/* Read Partial Sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp(BYTE *buff,   /* Pointer to the read buffer (NULL:Forward to the stream) */
                   DWORD sector, /* Sector number (LBA) */
                   UINT  offset, /* Byte offset to read from (0..511) */
                   UINT  count   /* Number of bytes to read (ofs + cnt mus be <= 512) */
)
{
	DRESULT res;
	BYTE    rc;
	UINT    bc;

	if (!(CardType & CT_BLOCK))
		sector *= 512; /* Convert to byte address if needed */

	res = RES_ERROR;
	if (send_cmd(CMD17, sector) == 0) { /* READ_SINGLE_BLOCK */

		// bc = 40000;	/* Time counter */
		do { /* Wait for response */
			rc = rcv_spi();
		} while (rc == 0xFF);

		if (rc == 0xFE) { /* A data packet arrived */

			bc = 512 + 2 - offset - count; /* Number of trailing bytes to skip */

			/* Skip leading bytes */
			while (offset--)
				rcv_spi();

			/* Receive a part of the sector */
			if (buff) { /* Store data to the memory */
				do {
					*buff++ = rcv_spi();
				} while (--count);
			} else { /* Forward data to the outgoing stream */
				do {
					// FORWARD(rcv_spi());
				} while (--count);
			}

			/* Skip trailing bytes and CRC */
			do
				rcv_spi();
			while (--bc);

			res = RES_OK;
		}
	}

	DESELECT();
	rcv_spi();

	return res;
}



#if PF_USE_WRITE
DRESULT disk_writep(const BYTE *buff, /* Pointer to the bytes to be written (NULL:Initiate/Finalize sector write) */
                    DWORD       sc    /* Number of bytes to send, Sector number (LBA) or zero */
)
{
	DRESULT     res;
	UINT        bc;
	static UINT wc; /* Sector write counter */

	res = RES_ERROR;

	if (buff) { /* Send data bytes */
		bc = sc;
		while (bc && wc) { /* Send data bytes to the card */
			xmit_spi(*buff++);
			wc--;
			bc--;
		}
		res = RES_OK;
	} else {
		if (sc) { /* Initiate sector write process */
			if (!(CardType & CT_BLOCK))
				sc *= 512;                  /* Convert to byte address if needed */
			if (send_cmd(CMD24, sc) == 0) { /* WRITE_SINGLE_BLOCK */
				xmit_spi(0xFF);
				xmit_spi(0xFE); /* Data block header */
				wc  = 512;      /* Set byte counter */
				res = RES_OK;
			}
		} else { /* Finalize sector write process */
			bc = wc + 2;
			while (bc--) {
				xmit_spi(0); /* Fill left bytes and CRC with zeros */
			}
			do {
				res = rcv_spi();
			} while (res == 0xFF);
			if ((res & 0x1F) == 0x05) { /* Receive data resp and wait for end of write process in timeout of 500ms */
				for (bc = 5000; rcv_spi() != 0xFF && bc; bc--) /* Wait for ready */
					sleep_us(100);
				if (bc)
					res = RES_OK;
			}
			DESELECT();
			rcv_spi();
		}
	}

	return res;
}
#endif
