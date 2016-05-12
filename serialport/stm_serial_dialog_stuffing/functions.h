#include <stm32l1xx.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_gpio.h>
#include <stm32l1xx_usart.h>
#include <misc.h>

#include "vars.h"

void Init();
int array_size(unsigned char* ar);
void clear_rx_buf();
void clear_tx_buf();
void delay(int msec);
void Send_USART1(unsigned char* data, int num);
int unpack_rx();
int pack_tx();
void reverse(unsigned char *data, int data_size);
void Nop();

