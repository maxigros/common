#include "vars.h"

unsigned char rx_buf[BUF_SIZZE];
unsigned char tx_buf[BUF_SIZZE];

UART_HandleTypeDef UartHandle;

int rx_count = 0;
int tx_count = 0;
unsigned char previous_byte = 0xff;
char flag_got_dle = 0;
unsigned short int package_counter = 0;
char pack_correct = 0;