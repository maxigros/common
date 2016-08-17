#include <stm32l1xx.h>
#include "vars.h"




int array_size(unsigned char* ar);
void clear_rx_buf();
void clear_tx_buf();
void delay(int msec);
//void Send_USART1(unsigned char* data, int num);
int unpack_rx();
int pack_tx();
void Nop();
//void Init_USART1();

