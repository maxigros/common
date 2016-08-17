#include "globals.h"

const char LLMDB_ADDR = 3;
const char ID [4] = "MGPS";
const unsigned short int SERIAL = 385;
const unsigned short int VERSION = 13;

unsigned short int meta_pos_reserved_1 = 0;
unsigned short int meta_pos_reserved_2 = 0;
unsigned short int meta_pos_reserved_3 = 0;

int int_for_binr = 0;

char new_page = 0;	// ????, ??? ????? ??????? ?? ????? ????????
int page_num_rec = 1;		// ????? ????????, ?? ??????? ??????? ??????
int symbol_pos = 0;
unsigned char spi_session_num[2];	// ????? ??????
unsigned char spi_session_addr[4];	// ?????? ??????
unsigned short int spi_meta_pos =6;	// ??????? ??? ?????? ? ????
unsigned char rate_flag = 0;	// ???? ?? 0, ?? ????? ???????? ???????? ?????
char BINR_stop_flag = 0;
char BINR_start_flag = 0;
char flag_send_to_llmdb = 0;
unsigned char BINR_cmd_inUse = 0;
char addr_done = 0;
char cmd_checked = 0;
char llmdb_oerr_flag = 0;
char SPI_active_flag = 0;
char SPI_activate = 0;
char SPI_disactivate = 0;
char SPI_active_reset =  0;
char SPI_session_in_process = 0;
char spi_stop = 0;
char spi_erase_flag = 0;
char binr_is_active = 0;
char binr_start_sent = 0;
char spi_no_space = 0;
char spi_fat_is_done = 0;
char binr_timer_is_active = 0;
char nv08_no_answer = 0;
unsigned char BINR_answer = 0x00;
unsigned char addr_rq = 0;

int rq_ptr = 0;
int rq_size = 0;
int ans_ptr = 0;
int ans_size = 0;
int rq_num = 0;
char act_buf = 0;
char flag_spi_need_push = 0;
char binr_state_act_buf = 0;
char spi_make_fat_flag = 0;

#pragma udata LDATA
unsigned char llmdb_rq[20];
unsigned char llmdb_ans[75];
unsigned char BINR_buf_60[14];

unsigned char BINR_state_1[72]; 
unsigned char BINR_state_2[72]; 
unsigned char spi_buf_receiving_A[528];
unsigned char spi_buf_receiving_B[528];
unsigned char spi_buf_toWrite[528];
unsigned char spi_buf_read[528];
unsigned char spi_meta[528];
#pragma udata


// DELAY
void delay(unsigned long int x)
{
	unsigned long int t;
	for (t = 0; t < x; t++);
}