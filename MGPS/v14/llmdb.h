#include "defines.h"	
#include "globals.h"
#include "spi.h"

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
