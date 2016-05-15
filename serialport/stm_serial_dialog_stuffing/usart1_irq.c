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
        
        if (flag_got_dle)
        {
            if (c != DLE)
                {
                    rx_buf[rx_count] = c;
                    rx_count++;
                }
            else
                {
                    // конец пакета
                    
                    
                    Nop();
                    tx_count = unpack_rx();
                    Nop();
              //      reverse(rx_buf, tx_count);
                    Nop();
//                    tx_count = rx_count;
    
                    // обработка команды
                    if (rx_count == 2)
                    {
                        pack_correct = 1;
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
                    // обычный пакет данных
                    else
                    {
                        char crc = rx_buf[0];
                        int s = tx_count;
                        for (int i = 1; i < s - 1; i++)
                            crc = crc ^ rx_buf[i];
                        if (crc == rx_buf[s - 1])
                            pack_correct = 1;
                            Nop();
                        if (pack_correct)
                            package_counter++;
                        //reverse(rx_buf, array_size(rx_buf));
                        
                        for (int i = 0; i < tx_count; i++)
                            tx_buf[i] = rx_buf[i];
                    }
                    
                    //tx_buf[0] = DLE;
                    //tx_buf[tx_count - 1] = DLE;
                    if (pack_correct)
                    {
                        int y = pack_tx();
                        //Send_USART1(tx_buf, tx_count);
                        Send_USART1(tx_buf, y);
                    }
                    flag_got_dle = 0;
                    rx_count = 0;
                    tx_count = 0;
                    pack_correct = 0;
                    clear_rx_buf();
                    clear_tx_buf();
                }
        }
        else
        {
            if (c == DLE)
                flag_got_dle = 1;
        } 
    }
}