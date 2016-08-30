#include "main.h"

char BINR_stop_str [] = {DLE, 0x0e, DLE, ETX};

/*******************************************************************************
 *  MAIN
 * *****************************************************************************
 */
void main(void) {
    int l, g;

    for (l = 0; l < 20; l++)
        llmdb_rq[l] = 0x00;

    for (l = 0; l < 90; l++) {
        BINR_state_1[l] = 0x00;
        BINR_state_2[l] = 0x00;
    }

    for (l = 0; l < SPI_PAGE_SIZE; l++) {
        spi_buf_read[l] = 0x00;
    }

    ADCON1 = 0x0F;

    // ??????? ??????????? LAT, ????? TRIS.
    // ???? ?????? ? ???????? ???????, ?? ?? ????? ???????? ???????? ??????? ?? ????? TXD.
    // ??? ??????????? ???, ??? ?? ????????? ? LAT ???????? ?????????????? ????????.

    LATC = 0xFF;
    TRISCbits.RC6 = 0; // UART1 TX
    TRISCbits.RC7 = 1; // UART1 RX

    LATG = 0xFF;
    // ????? ??????????? ??????????????? ? N ??????, ??? N - ?????????? ????????? ? bits.
    TRISGbits.RG1 = 0; // UART2 TX
    TRISGbits.RG2 = 1; // UART2 RX

    // ???? ???????????? ??????????, ?? ?????? ????? ?????????????? ?????? ??? ???????.
    // ???? ?? ?????????????? bits, ?? ?????? ???? ?? ???, ? ?? ???.
    // bits ??? ?????????? ????? ?????? bsf/bcf, ???? ? ???????? ???????? ???????????? ?????????.
    LATA = (0 << RS485DE) | (0 << RS485REn) | (1 << LED); // ?????????, ?????????? ??? ??????
    TRISA = 0;


    //==================================		USART		=================================
    //==============================================================================

    SPBRG1 = baud_17;
    SPBRGH1 = 0;
    BAUDCON1 = (1 << BAUDCON_BRG16);
    TXSTA1 = (1 << TXSTA_TX9) | (1 << TXSTA_TXEN) | (1 << TXSTA_BRGH);
    RCSTA1 = (1 << RCSTA_SPEN) | (1 << RCSTA_RX9) | (1 << RCSTA_CREN) | (0 << RCSTA_ADDEN);

    SPBRG2 = baud_25;
    SPBRGH2 = 0;
    BAUDCON2 = (1 << BAUDCON_BRG16);
    TXSTA2 = (1 << TXSTA_TX9) | (1 << TXSTA_TXEN) | (1 << TXSTA_BRGH);
    RCSTA2 = (1 << RCSTA_SPEN) | (1 << RCSTA_RX9) | (1 << RCSTA_CREN) | (0 << RCSTA_ADDEN);

    //==============================================================================
    //==============================================================================

    //????????? ??????????

    RCONbits.IPEN = 1; // ???????????????? ?????????? 
    INTCONbits.GIEH = 1; // ?????????? ?????????? 
    INTCONbits.PEIE = 1; // ???????????? ?????????? ?????????

    PIE1bits.RC1IE = 1; // Enable USART1 RX interrupt
    IPR1bits.RC1IP = 0; // USART1 receive interrupt priority

    PIE3bits.RC2IE = 1; // Enable USART2 RX interrupt
    IPR3bits.RC2IP = 1; // USART2 receive interrupt priority

    INTCONbits.TMR0IE = 1; // enable прерывания таймера
    INTCON2bits.TMR0IP = 0; // приоритет прерывания таймера

    //==================================		SPI		=================================
    //==============================================================================

    TRISCbits.RC2 = 0; // CS output
    TRISCbits.RC3 = 0; // SDO output
    TRISCbits.RC4 = 1; // SDI input
    TRISCbits.RC5 = 0; // SCK output

    SPI_Init();

    //==============================================================================
    //==============================================================================

    SendBinr(BINR_stop_str, 4);

    act_buf = 1; // для буферов SPI
    binr_state_act_buf = 0; // для буферов вектора состояния

    RefreshMeta();
    if (spi_meta[0] == ID[0] && spi_meta[1] == ID[1] && spi_meta[2] == ID[2] && spi_meta[3] == ID[3])
        spi_fat_is_done = 1;
    else
        spi_fat_is_done = 0;

    page_num_rec = 1;
    if (spi_fat_is_done) {
        for (l = 6; l < SPI_PAGE_SIZE - 5; l += 6) {
            if ((spi_meta[l] == 0x00) && (spi_meta[l + 1] == 0x00)) {
                if (l == 12) {
                    if (spi_meta[l - 2] == 0xFF) {
                        spi_meta_pos = 6;
                        page_num_rec = 1;
                    }
                } else {
                    //                    spi_meta_pos = l;
                    if (l > 6) {
                        for (g = l - 2; g > 9; g -= 6) {
                            if (spi_meta[g] != 0xFF) {
                                spi_meta_pos = g + 2;
                                page_num_rec = (((int) spi_meta[spi_meta_pos - 2] << 8) | (int) spi_meta[spi_meta_pos - 1]) + 1;
                                break;
                            }
                        }
                    }
                }
                break;
            }
        }
    }


    Nop();

    PORTFbits.RF3 = 0;

    while (1) {
        if (flag_spi_need_push && (!SPI_disactivate)) {
            
            if (!act_buf)
                // если в данный момент запись идет в буфер А, то "пушить" надо буфер Б
            {
                SPI_CS = 0; //CSN high	
                SPI_WR(0x86);
                SPI_WR((page_num_rec >> 6) & 0xFF);
                SPI_WR((page_num_rec << 2) & 0xFF);
                SPI_WR(0x00);
                SPI_CS = 1; // CSN low
            } else {
                SPI_CS = 0; //CSN high	
                SPI_WR(0x83);
                SPI_WR((page_num_rec >> 6) & 0xFF);
                SPI_WR((page_num_rec << 2) & 0xFF);
                SPI_WR(0x00);
                SPI_CS = 1; // CSN low
            }
            page_num_rec++;
            symbol_pos = 0;
            if (page_num_rec == SPI_PAGES_NUM / 100 * 90) 
            {
                spi_few_space = 1;
            }
            
            if (page_num_rec == SPI_PAGES_NUM - 2)
            {
                spi_no_space = 1;
            }
            
            flag_spi_need_push = 0;
            push_saved = 1;
        }

        if (spi_erase_flag) 
        {
            spi_erase_flag = 0;
            SpiChipErase();
        }

        if (spi_make_fat_flag) 
        {
            spi_make_fat_flag = 0;
            SpiMakeFat();
        }

        if (SPI_activate) 
        {
            SPI_activate = 0;
            start_spi();
        }

        if (spi_no_space)
        {
            if (SPI_active_flag) {
                SPI_disactivate = 1;
            }
        }
        
        if (SPI_disactivate) 
        {
            SPI_disactivate = 0;
            stop_spi();
        }

        if (SPI_active_reset) 
        {
            SPI_active_reset = 0;
            restart_spi();
        }



        if (addr_rq && (!addr_done)) {
            if (addr_rq == LLMDB_ADDR) {
                addr_done = 1;
                llmdb_rq[0] = LLMDB_ADDR;
            } else {
                clearBuffer_rq();
                addr_rq = 0;
                rq_ptr = 0;
            }
        }
        if (llmdb_oerr_flag) {
            llmdb_oerr_flag = 0;
            clearBuffer_rq();
            addr_done = 0;
            addr_rq = 0;
            cmd_checked = 0;
            RCSTA2bits.CREN = 0;
            RCSTA2bits.CREN = 1;
        }
        if (rq_ptr) {
            if ((rq_ptr >= 2) && (addr_done) && (!cmd_checked)) {
                switch (llmdb_rq[1]) {
                    case CMD_LLMDB_TEST:
                        rq_size = SZRX_LLMDB_TEST;
                        ans_size = SZTX_LLMDB_TEST;
                        ans_ptr = 0;
                        break;
                    case CMD_VERSION:
                        rq_size = SZRX_VERSION;
                        ans_size = SZTX_VERSION;
                        ans_ptr = 0;
                        break;
                    case CMD_BINR_GETSTATE:
                        rq_size = SZRX_BINR_GETSTATE;
                        ans_size = SZTX_BINR_GETSTATE;
                        ans_ptr = 0;
                        break;
                    case CMD_BINR_GET_FULL_STATE:
                        rq_size = SZRX_BINR_GET_FULL_STATE;
                        ans_size = SZTX_BINR_GET_FULL_STATE;
                        ans_ptr = 0;
                        break;
                    case CMD_ID:
                        rq_size = SZRX_ID;
                        ans_size = SZTX_ID;
                        ans_ptr = 0;
                        break;
                    case CMD_SERIAL:
                        rq_size = SZRX_SERIAL;
                        ans_size = SZTX_SERIAL;
                        ans_ptr = 0;
                        break;                  
                    case CMD_LLMDB_RATE:
                        rq_size = SZRX_LLMDB_RATE;
                        ans_size = SZTX_LLMDB_RATE;
                        ans_ptr = 0;
                        break;
                    case CMD_SPI_FSPACE:
                        rq_size = SZRX_SPI_FSPACE;
                        ans_size = SZTX_SPI_FSPACE;
                        ans_ptr = 0;
                        break;
                    case CMD_DEV_STATUS:
                        rq_size = SZRX_DEV_STATUS;
                        ans_size = SZTX_DEV_STATUS;
                        ans_ptr = 0;
                        break;
                    case CMD_BINR_START:
                        rq_size = SZRX_BINR_START;
                        ans_size = SZTX_BINR_START;
                        ans_ptr = 0;
                        break;
                    case CMD_BINR_STOP:
                        rq_size = SZRX_BINR_STOP;
                        ans_size = SZTX_BINR_STOP;
                        ans_ptr = 0;
                        break;
                    case CMD_SPI_START:
                        rq_size = SZRX_SPI_START;
                        ans_size = SZTX_SPI_START;
                        ans_ptr = 0;
                        break;
                    case CMD_SPI_STOP:
                        rq_size = SZRX_SPI_STOP;
                        ans_size = SZTX_SPI_STOP;
                        ans_ptr = 0;
                        break;
                    case CMD_SPI_ERASE:
                        rq_size = SZRX_SPI_ERASE;
                        ans_size = SZTX_SPI_ERASE;
                        ans_ptr = 0;
                        break;
                    case CMD_SPI_MAKE_FAT:
                        rq_size = SZRX_SPI_MAKE_FAT;
                        ans_size = SZTX_SPI_MAKE_FAT;
                        ans_ptr = 0;
                        break;
                    case CMD_SPI_SESSION_SIZE:
                        rq_size = SZRX_SPI_SESSION_SIZE;
                        ans_size = SZTX_SPI_SESSION_SIZE;
                        ans_ptr = 0;
                        break;
                    case CMD_SPI_SESSION_BLOCK:
                        rq_size = SZRX_SPI_SESSION_BLOCK;
                        ans_size = SZTX_SPI_SESSION_BLOCK;
                        ans_ptr = 0;
                        break;
                    case CMD_SPI_READ:
                        rq_size = SZRX_SPI_READ;
                        ans_size = SZTX_SPI_READ;
                        ans_ptr = 0;
                        break;
                    case CMD_SPI_READ_BLOCK:
                        rq_size = SZRX_SPI_READ_BLOCK;
                        ans_size = SZTX_SPI_READ_BLOCK;
                        ans_ptr = 0;
                        break;
                    default:
                        // ??????????? ???????, ????????? ??
                        clearBuffer_rq();
                        addr_done = 0;
                        addr_rq = 0;
                        //RCSTA2bits.ADDEN = 1;
                        break;
                }
                if (addr_done)
                    cmd_checked = 1;
            }
            if (cmd_checked) {
                if ((rq_ptr == rq_size) && (rq_ptr)) {
                    // ???????? ????? ?????????, ????? ?????????
                    if (checkCRC_rq() == 0) {
                        // ?????? ??????????? ?????
                        clearBuffer_rq();
                        addr_done = 0;
                        addr_rq = 0;
                        cmd_checked = 0;
                        //RCSTA2bits.ADDEN = 1;
                    }
                    else
                        Send_to_LLMDB();
                }
            }
        }

        if (BINR_start_flag) {
            BINR_start_flag = 0;
            binr_start_sent = 1;
            BINR_start();
        }

        if (BINR_stop_flag) {
            BINR_stop_flag = 0;
            binr_start_sent = 0;
            SendBinr(BINR_stop_str, 4);
        }

        if (rate_flag) {
            switch (rate_flag) {
                case 0x19:
                    SPBRG2 = baud_25;
                    break;
                case 0x17:
                    SPBRG2 = baud_23;
                    break;
                case 0x11:
                    SPBRG2 = baud_17;
                    break;
                case 0x10:
                    SPBRG2 = baud_16;
                    break;
                case 0x0e:
                    SPBRG2 = baud_14;
                    break;
                case 0x0d:
                    SPBRG2 = baud_13;
                    break;
            }
            rate_flag = 0;
        }
    }
}

