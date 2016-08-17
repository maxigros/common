#include "globals.h"

/*
 ********** FLAGS   **********
 */
// need to copy data from "saved" array to flash memory
char push_saved = 0;
// need to change LLMDB baudrate
unsigned char rate_flag = 0;
// need to stop NV08 packages
char BINR_stop_flag = 0;
// need to start NV08 packages
char BINR_start_flag = 0;
// LLMDB address is checked and correct
char addr_done = 0;
// LLMDB command is checked
char cmd_checked = 0;
// LLMDB overrun flag
char llmdb_oerr_flag = 0;
// Data is being recorded to flash
char SPI_active_flag = 0;
// Need to start session
char SPI_activate = 0;
// Need to stop session
char SPI_disactivate = 0;
// Need to stop session and start a new one
char SPI_active_reset =  0;
// Session is being recorded
char SPI_session_in_process = 0;
// Need to erase flash
char spi_erase_flag = 0;
// Data is neing sent from NV08
char binr_is_active = 0;
// BINR cmd "CMD_BINR_START" has already been sent
char binr_start_sent = 0;
// No free apace on flash
char spi_no_space = 0;
// 10% of free space left on flash
char spi_few_space = 0;
// File system is set
char spi_fat_is_done = 0;
// BINR timer. Used for flag "binr_is_active" 
char binr_timer_is_active = 0;
// Need to make fie system
char spi_make_fat_flag = 0;
// Need to copy data from flash buffer to flah memory
char flag_spi_need_push = 0;


/*
 ********** VARIABLES   ****************
 */

// devide address
const char LLMDB_ADDR = 3;
// device identifier
const char ID [4] = {'M', 'G', 'P', 'S'}; // !!!
// device serial number
const unsigned short int SERIAL = 385;
// version of software
const unsigned short int VERSION = 14;
// symbol position in "saved" array
int symbol_pos_saved = 0;
// number of page on flash which data is being written to
int page_num_rec = 1;
// symbol position in flash buffer
int symbol_pos = 0;
// name of session which is going to be recorded
unsigned char spi_session_num[2];
// addresses in "spi_meta" array of session which is going to be recorded
unsigned char spi_session_addr[4];
// symbol position inr "spi_meta" array
unsigned short int spi_meta_pos =6;
// ID of BINR package which is being received
unsigned char BINR_cmd_inUse = 0;

unsigned char BINR_answer = 0x00;
// Received LLMDN address which is needed to be checked 
unsigned char addr_rq = 0;
// Symbol position in LLMDB request
int rq_ptr = 0;
// Size of LLMDB request
int rq_size = 0;
// Symbol position in LLMDB answer message
int ans_ptr = 0;
// Size of LLMDN answer message
int ans_size = 0;
// Number of flash buffer used for recording
char act_buf = 0;
// Number of buffer used for recording of BINR state vector (BINR_state_n) 
char binr_state_act_buf = 0;



#pragma udata LDATA
// Array for LLMDB request
unsigned char llmdb_rq[20];
// Array for LLMDB answer
unsigned char llmdb_ans[90];
// Array 1 for BINR state vector
unsigned char BINR_state_1[90]; 
// Array 2 for BINR state vector
unsigned char BINR_state_2[90]; 
// Array for backup of BINR bytes
unsigned char saved[100];
// Array for data read from flash
unsigned char spi_buf_read[SPI_PAGE_SIZE];
// Array for file system
unsigned char spi_meta[SPI_PAGE_SIZE];
#pragma udata


// DELAY
void delay(unsigned long int x)
{
	unsigned long int t;
	for (t = 0; t < x; t++);
}