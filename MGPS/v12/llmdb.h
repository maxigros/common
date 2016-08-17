#include "defines.h"	
#include "globals.h"
#include "spi.h"

/*******************************************************************************
 *  EXTERN VARIABLES
 * *****************************************************************************
 */
extern unsigned char rate_flag;	// ???? ?? 0, ?? ????? ???????? ???????? ?????
extern char SPI_active_flag;
extern char SPI_active_reset;
extern char SPI_activate;
extern char SPI_disactivate;
extern char spi_erase_flag;
extern char spi_inavailable;
extern char BINR_stop_flag;
extern char BINR_start_flag;
extern char cmd_checked;
extern unsigned char addr_rq;
extern char addr_done;
extern char flag_send_to_llmdb;
extern int rq_ptr;
extern int rq_size;
extern const char LLMDB_ADDR;

extern unsigned char spi_session_num[2];	// ????? ??????

extern unsigned char llmdb_rq[20];
extern unsigned char llmdb_ans[75];
extern unsigned char BINR_state[72];
extern unsigned char spi_meta[528];
extern unsigned char spi_buf_read[528];
/*******************************************************************************
 *  VARIABLES
 * *****************************************************************************
 */
//char ccs;	// ??????????? ????? ?????? LLMDB
//char cmd;	// ??????? LLMDB
//int a;		// ??? ????? ??????????
//int count;
//char session_already_exists = 0;
//unsigned short int page_num_read = 0;	// ????? ???????? ??? ??????
//char spi_session_size_success = 0;
//unsigned long int spi_session_size = 0;
//char spi_session_block_success = 0;
//unsigned long int spi_block_num = 0;
//unsigned short int spi_sec_num = 0;	// ???? ?? 22 ?????? ???????? ?? 24 ?????
//unsigned long int spi_free_space = 0;
//unsigned short int llmdb_sec_num;

/*******************************************************************************
 *  PROTOTYPES
 * *****************************************************************************
 */
void Send_to_LLMDB();
void clearBuffer_rq();
char checkCRC_rq();
void makeCRC_ans();
