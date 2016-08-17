#include "defines.h"
#include "globals.h"
#include "binr.h"

#define SPI_WR(x) \
	SSPBUF = x; \
	while(!SSPSTATbits.BF); 

#define SPI_RDWR(x, y) \
	SSPBUF = x; \
	while(!SSPSTATbits.BF); \
	y = SSPBUF;

/*******************************************************************************
 *  PROTOTYPES
 * *****************************************************************************
 */
void SPI_Init();
void SpiCpyBufToMem(int n);
void SpiWriteMem(int n);
void SpiReadMem(int n);
void RefreshMeta();
void UploadMeta();
unsigned long int Spi_FSpace(unsigned char b1, unsigned char b2);
unsigned long int Spi_Session_Size(unsigned char b1);
unsigned char  SpiStatus();
void start_spi();
void stop_spi();
void restart_spi();
void SpiChipErase();
void SpiMakeFat();
void W_Dummy(int p);
void countSPIBeginVars();
