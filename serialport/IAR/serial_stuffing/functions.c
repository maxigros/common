#include "functions.h"

void Init_USART1()
{  

    
    /* config interrupts
    */
    
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
//        USART_SendData(USART1, data[i]);
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
    for (int i = 0; i < BUF_SIZZE; i++)
                rx_buf[i] = 0x00;
}
void clear_tx_buf()
{
    for (int i = 0; i < BUF_SIZZE; i++)
                tx_buf[i] = 0x00;
}

int pack_tx()
{
    unsigned char temp[BUF_SIZZE];
    for (int i = 0; i < BUF_SIZZE; i++)
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
    int s = rx_count;
    Nop();
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
            rx_buf[s - 1] = 0;
            s--;    
        }
    return s;    
}