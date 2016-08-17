#include "defines.h"
/******************************************************************************
 *  DEVICE INFORMATION
 * ****************************************************************************
 */
extern const char LLMDB_ADDR;
extern const char ID [4];
extern const unsigned short int SERIAL;
extern const unsigned short int VERSION;

/******************************************************************************
 *  VARIABLES
 * ****************************************************************************
 */

extern char push_saved;
extern int symbol_pos_saved;
extern int page_num_rec;		
extern int symbol_pos;
extern unsigned char spi_session_num[2];	
extern unsigned char spi_session_addr[4];	
extern unsigned short int spi_meta_pos;	
extern unsigned char rate_flag;	
extern char BINR_stop_flag;
extern char BINR_start_flag;
extern unsigned char BINR_cmd_inUse;
extern char addr_done;
extern char cmd_checked;
extern char llmdb_oerr_flag;
extern char SPI_active_flag;
extern char SPI_activate;
extern char SPI_disactivate;
extern char SPI_active_reset;
extern char SPI_session_in_process;
extern char spi_stop;
extern char spi_erase_flag;
extern char binr_is_active;
extern char binr_start_sent;
extern char spi_no_space;
extern char spi_few_space;
extern char spi_fat_is_done;
extern char binr_timer_is_active;
extern unsigned char BINR_answer;
extern unsigned char addr_rq;
extern int rq_ptr;
extern int rq_size;
extern int ans_ptr;
extern int ans_size;
extern char act_buf;
extern char flag_spi_need_push;
extern char binr_state_act_buf;
extern char spi_make_fat_flag;

#pragma udata LDATA
extern unsigned char llmdb_rq[20];
extern unsigned char llmdb_ans[90];
extern unsigned char BINR_state_1[90]; 
extern unsigned char BINR_state_2[90]; 
extern unsigned char saved[100];
extern unsigned char spi_buf_read[SPI_PAGE_SIZE];
extern unsigned char spi_meta[SPI_PAGE_SIZE];
#pragma udata

/******************************************************************************
 *  PROTOTYPES
 * ****************************************************************************
 */
void delay(unsigned long int x);

