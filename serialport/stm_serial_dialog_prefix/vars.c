#include "vars.h"

unsigned char rx_buf[BUF_SIZZE + PREFIX_SIZE + 2];
unsigned char tx_buf[BUF_SIZZE + PREFIX_SIZE + 2];

unsigned char prefix_buf[PREFIX_SIZE];
int prefix_counter = 0;

int rx_count = 0;
int tx_count = 0;
unsigned char previous_byte = 0x00;
char flag_got_prefix = 0;
unsigned short int package_counter = 0;
char pack_correct = 0;
unsigned short int pack_size = 0;

GPIO_InitTypeDef port;
USART_InitTypeDef usart;
NVIC_InitTypeDef nvic;