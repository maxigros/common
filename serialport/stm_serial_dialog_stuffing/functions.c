#include "functions.h"

void Init()
{
	// clocks
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB , ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE);
	//------------------------------------------------------------------
	/* config PORT B (defaults: GPIO_OType_PP
								GPIO_PuPd_NOPULL)
	*/
	
		// LEDs  (pins 6 and 7)
	port.GPIO_Mode = GPIO_Mode_OUT;
	port.GPIO_Pin = LED_GREEN | LED_BLUE;
	port.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &port);		
	
	//-------------------------------------------------------------------
	/* config PORT A
	*/
	
		// Button
	port.GPIO_Mode = GPIO_Mode_IN;
	port.GPIO_Pin = BUTTON;
	port.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &port);
	
		// USART1
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
			// TX
	port.GPIO_Mode = GPIO_Mode_AF;
	port.GPIO_PuPd = GPIO_PuPd_UP;
	port.GPIO_Pin = GPIO_Pin_9;
	port.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &port);
	
			// RX
	port.GPIO_Mode = GPIO_Mode_AF;
	port.GPIO_PuPd = GPIO_PuPd_NOPULL;
	port.GPIO_Pin = GPIO_Pin_10;
	port.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &port);
	
	/*	USART1
				defaults:
                        - BaudRate = 9600 baud  
                        - Word Length = 8 Bits
                        - One Stop Bit
						- No parity
						- Hardware flow control disabled (RTS and CTS signals)
						- Receive and transmit enabled
	*/
	
	USART_StructInit(&usart);
	usart.USART_BaudRate = BAUDRATE;
	//usart.USART_Parity = USART_Parity_Odd;
	USART_Init(USART1, &usart);	
    
    /* config interrupts
    */
    nvic.NVIC_IRQChannel = USART1_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}

void delay(int msec)
{
	int k = msec * 1000;
	for (;k > 0; k--);
}

void Nop()
{
    for (int i = 0; i < 1; i++);
}

void Send_USART1(unsigned char* data, int num)
{
    for (int i = 0; i < num; i++)
    {
        USART_SendData(USART1, data[i]);
        for (int j = 0; j < 500; j++)
            __NOP();
    }
}

int array_size(unsigned char* ar)
{
    int num = 0;
    for (int i = 0;; i++)
        if (ar[i] == 0x00)
        {
            num = i;
            break;
        }
    return num;    
}

void clear_rx_buf()
{
    for (int i = 0; i < 10; i++)
                rx_buf[i] = 0x00;
}
void clear_tx_buf()
{
    for (int i = 0; i < 10; i++)
                tx_buf[i] = 0x00;
}

int pack_tx()
{
    unsigned char temp[1000];
    for (int i = 0; i < 1000; i++)
        temp[i] = 0xFF;
        
    for (int i = 0; i < tx_count; i++)
        temp[i] = tx_buf[i];
    Nop();
    tx_buf[0] = DLE;
    int pos = 1;
    for (int i = 0; i < tx_count; i++)
    {
        if (temp[i] == ESC || temp[i] == DLE)
        {
            tx_buf[pos] = ESC;
            pos++;
            // инвертирование 5 бита
            if (temp[i] & (1 << 5))  // если 5 бит ненулевой
                tx_buf[pos] = temp[i] & (~(1 << 5));
            else
                tx_buf[pos] = temp[i] | (1 << 5);
        }
        else
            tx_buf[pos] = temp[i];
        pos++;
    }
    Nop();
    tx_buf[pos] = DLE;
    tx_count = pos + 1;
    return tx_count;
}

int unpack_rx()
{
    int s = array_size(rx_buf);
    for (int i = 0; i < s; i++)
        if (rx_buf[i] == ESC)
        {
            // инвертирование 5 бита
            if (rx_buf[i + 1] & (1 << 5))  // если 5 бит ненулевой
                rx_buf[i + 1] = rx_buf[i + 1] & (~(1 << 5));
            else
                rx_buf[i + 1] = rx_buf[i + 1] | (1 << 5);
                
            // смещение массива    
            for (int j = i; j < s - 1; j++)
                rx_buf[j] = rx_buf[j + 1];
            s--;    
        }
    return s;    
}

void reverse(unsigned char *data, int data_size)
{
    // реверс
    unsigned char temp[data_size];
    for (int i = 0; i < data_size; i++)
        temp[i] = rx_buf[data_size - 1 - i];
    for (int i = 0; i < data_size; i++)
        rx_buf[i] = temp[i];
}