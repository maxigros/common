#include "vars.h"
#include "functions.h"

void USART1_IRQHandler()
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE))
    {
        // clear interrupt flag
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        
        unsigned char c;
        c = USART_ReceiveData(USART1);
        
        
            
        if (flag_got_prefix)
        {
            rx_buf[rx_count] = c;
            rx_count++;
            if (rx_count == 2)
                pack_size = ((int)rx_buf[0] >> 8) | (int)rx_buf[1];
            else
            {
                // full package received 
                if (rx_count == pack_size + 2)
                {
                    unsigned char temp_buf[pack_size];
                    if (pack_size == 2)
                    {
                        //  CMDs
                        switch (rx_buf[0])
                        {
                            case CMD_HELLO:
                                //tx_count += 1;
                                for (int i = 1; i < tx_count - 1; i++)
                                    tx_buf[i] = 0xEE;
                                break;
                            case CMD_COUNTER:
                                //tx_count += 1;
                                tx_buf[1] = package_counter & 0xFF;
                                tx_buf[0] = (package_counter >> 8) & 0xFF;
                                break;
                            case CMD_COUNTER_ZERO:
                                tx_count--;
                                package_counter = 0;
                                tx_buf[0] = CMD_COUNTER_ZERO;
                                break;
                        }
                    }
                    else
                    {
                        for (int i = 0; i < pack_size; i++)
                            temp_buf[i] = rx_buf[i + 2];
                            
                        // CRC
                        char crc = temp_buf[0];
                        for (int i = 1; i < pack_size - 1; i++)
                            crc ^= temp_buf[i];
                        if (crc == temp_buf[pack_size - 1])
                             pack_correct = 1;
                    }
                    if (pack_correct)
                    {
                        package_counter++;
                        int k = pack_tx(temp_buf, pack_size);
                        Send_USART1(tx_buf, k);
                    }
                    
                    Nop();
                    pack_size = 0;
                    rx_count = 0;
                    flag_got_prefix = 0;
                    prefix_counter = 0;
                }
            }
        }
        else
        {
            if (c == PREFIX_BYTE)
            {
                if (previous_byte == PREFIX_BYTE)
                    prefix_counter++;
            }
            else
                prefix_counter = 0;
            if (prefix_counter == PREFIX_SIZE - 1)
                flag_got_prefix = 1;
        }
        
        previous_byte = c;
    }
}