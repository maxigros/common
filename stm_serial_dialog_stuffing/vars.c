#include "vars.h"

unsigned char rx_buf[1000];
unsigned char tx_buf[1000];

int rx_count = 0;
int tx_count = 0;
unsigned char previous_byte = 0xff;
char flag_got_dle = 0;
unsigned short int package_counter = 0;

GPIO_InitTypeDef port;
USART_InitTypeDef usart;
NVIC_InitTypeDef nvic;