#include "llmdb.h"
#include "spi.h"

void Send_to_LLMDB()
{
    char ccs;
    char cmd;
    char status_byte;
    int a, g;
    int count;
    
    char session_already_exists = 0;
    unsigned short int page_num_read = 0;	// номер страницы для чтения
    char spi_session_size_success = 0;
    unsigned long int spi_session_size = 0;
    char spi_session_block_success = 0;
    unsigned long int spi_block_num = 0;
    unsigned short int spi_sec_num = 0;	// ???? ?? 22 ?????? ???????? ?? 24 ?????
    unsigned long int spi_free_space = 0;
    unsigned short int llmdb_sec_num;
    
    char eeprom_buf[4];
    
	cmd = llmdb_rq[1];
	ccs = ((cmd>>3) + cmd) & 0x0F;

	for (a = 0; a < 32; a++)
		llmdb_ans[a] = 0x00;

	if(cmd == CMD_VERSION)
	{
		ccs |= (0x03 << 4);
		llmdb_ans[0] = ccs;
		llmdb_ans[1] = VERSION;
		llmdb_ans[2] = VERSION >> 8;
	}

	if(cmd == CMD_SERIAL)
	{
		ccs |= (0x03 << 4);
		llmdb_ans[0] = ccs;	
		llmdb_ans[1] = SERIAL;	
		llmdb_ans[2] = SERIAL>>8;
	}
    ///////////
    if(cmd == CMD_META_POS)
	{
		ccs |= (0x03 << 4);
		llmdb_ans[0] = ccs;	
		llmdb_ans[1] = meta_pos_reserved_1;	
		llmdb_ans[2] = meta_pos_reserved_1 >> 8;
        llmdb_ans[3] = meta_pos_reserved_2;	
		llmdb_ans[4] = meta_pos_reserved_2 >> 8;
        llmdb_ans[5] = meta_pos_reserved_3;	
		llmdb_ans[6] = meta_pos_reserved_3 >> 8;
	}
///////////////////////
	if(cmd == CMD_ID)
	{
		ccs |= (0x03 << 4);
		llmdb_ans[0] = ccs;
//		for(a = 0; a<strlen(ID); a++)
//		{
//			llmdb_ans[a+1] = ID[a];
//		}
        llmdb_ans[1] = ID[3];
        llmdb_ans[2] = ID[2];
        llmdb_ans[3] = ID[1];
        llmdb_ans[4] = ID[0];
		for(a=strlen(ID)+1; a<SZTX_ID; a++)
			llmdb_ans[a] = 0;
	}

	if(cmd == CMD_LLMDB_RATE)
	{
		ccs |= (0x00 << 4);
		llmdb_ans[0] = ccs;		
		rate_flag = llmdb_rq[2];
	}

	if(cmd == CMD_SPI_START)
	{	
        int i;
		RefreshMeta();
		for (i= 6; i < 523; i+=6)
			if (spi_meta[i] == llmdb_rq[2] && spi_meta[i+1] == llmdb_rq[3])
			{
				session_already_exists = 1;
				break;
			}

		if (!session_already_exists)
		{
			ccs |= (0x00 << 4);

			spi_session_num[0] = llmdb_rq[2];
			spi_session_num[1] = llmdb_rq[3];
	
			if (SPI_active_flag)
			{
				SPI_active_reset = 1;
			}
			else
			{
				SPI_activate = 1;
			}
		}
		else
        {
			ccs |= (0x0e << 4);
            session_already_exists = 0;
        }

		llmdb_ans[0] = ccs;
	}

	if(cmd == CMD_SPI_STOP)
	{
		ccs |= (0x00 << 4);
		llmdb_ans[0] = ccs;
        if (SPI_active_flag)
        {
            SPI_disactivate = 1;
        }
	}
    
    if(cmd == CMD_DEV_STATUS)
	{
        ccs |= (0x03 << 4);
		llmdb_ans[0] = ccs;
        
        
        status_byte = 0;
        
//   0    // включен ли binr
        if (binr_is_active) 
        {
            status_byte |= 1;
        }
        
//   1     // доступна ли флешка. Если 1, то все ОК
        status_byte |= ((int)SpiStatus()) << 1;  
        
//   2     // наличие файловой системы
        RefreshMeta();
        if (spi_meta[0] == ID[0] && spi_meta[1] == ID[1] && spi_meta[2] == ID[2] && spi_meta[3] == ID[3])
            status_byte |= (1 << 2);
        
//   3    // на флешке есть место
        if (!spi_no_space)   
        {
            status_byte |= (1 << 3);
        }
        
//   4    // работоспособность флешки (если 1, то все норм)
        for (a = 0; a < 4; a++)
            eeprom_buf[a] = 0;
        SPI_CS= 0;            //CSN low	
        SPI_WR(0x9F);
        for (a = 0; a < 4; a++)
        {
            SPI_RDWR(0x00,eeprom_buf[a]); 
        }
        SPI_CS = 1;                    // CSN high
        if ( eeprom_buf[0] == 0x1F && eeprom_buf[1] == 0x27 && eeprom_buf[2] == 0x01)
            status_byte |= (1 << 4);
        
//   5    //BINR_START была отправлена 
        if (binr_start_sent)   
        {
            status_byte |= (1 << 5);
        }
        else
        {
            status_byte |= (0 << 5);
        }
        
//   6    //идет запись сессии
        if (SPI_session_in_process)   
        {
            status_byte |= (1 << 6);
        }
        else
        {
            status_byte |= (0 << 6);
        }

		llmdb_ans[1] = status_byte;
	}

	if(cmd == CMD_SPI_ERASE)
	{
		ccs |= (0x00 << 4);
		llmdb_ans[0] = ccs;
		
		spi_erase_flag = 1;
	}
    
    if(cmd == CMD_SPI_MAKE_FAT)
	{
		ccs |= (0x00 << 4);
		llmdb_ans[0] = ccs;
		
		spi_make_fat_flag = 1;
	}

	if(cmd == CMD_SPI_SESSION_SIZE)
	{
		spi_session_num[0] = llmdb_rq[2];
		spi_session_num[1] = llmdb_rq[3];
		RefreshMeta();
		for (a = 0; a < 523; a+=6 )
		{
			if (spi_meta[a] == spi_session_num[0] && spi_meta[a+1] == spi_session_num[1])
			{
				spi_session_size_success = 1;
				spi_session_size = Spi_Session_Size(a);
				break;
			}
		}

		if (spi_session_size_success)
		{
			ccs |= (0x03 << 4);
			llmdb_ans[4] = spi_session_size >> 24;
			llmdb_ans[3] = spi_session_size >> 16;
			llmdb_ans[2] = spi_session_size >> 8;
			llmdb_ans[1] = spi_session_size;
		}
		else
		{
			ccs |= (0x0e << 4);
			llmdb_ans[1] = 0x00;
			llmdb_ans[2] = 0x00;
			llmdb_ans[3] = 0x00;
			llmdb_ans[4] = 0x00;
		}
		llmdb_ans[0] = ccs;
		spi_session_size_success = 0;
	}

	if(cmd == CMD_SPI_SESSION_BLOCK)
	{
        // Смотрим номер сессии, которую будем читать
        Nop();
		spi_session_num[0] = llmdb_rq[2];
		spi_session_num[1] = llmdb_rq[3];
        Nop();
        // Собираем номер блока данной сессии для чтения
		spi_block_num = ((unsigned long int)llmdb_rq[7] << 24) | 
                        ((unsigned long int)llmdb_rq[6] << 16) |
						((unsigned long int)llmdb_rq[5] << 8) | 
                         (unsigned long int)llmdb_rq[4];
        // Обновляем мету
        Nop();
		RefreshMeta();
        Nop();
        // Ищем название сессии в мете
		for (a = 6; a < 523; a+=6 )
		{
            Nop();
			if (spi_meta[a] == spi_session_num[0] && spi_meta[a+1] == spi_session_num[1])
			{
                Nop();
                // Вычисляем размер сессии
				spi_session_size = Spi_Session_Size(a);
                // Проверяем, есть ли пришедший номер блока в данной сессии. 
				if ((spi_session_size / 24) >= spi_block_num && spi_block_num > 0)
				{
                    Nop();
					spi_session_block_success = 1;
                    // Ставим указатель на страницу, с которой начинается данная сессия
					page_num_read = ((int)spi_meta[a + 2] << 8) | (int)spi_meta[a+3];
                    Nop();
				}
				break;
			}
		}
        // Если блок действительно есть в этой сессии
		if (spi_session_block_success)
		{
            Nop();
			ccs |= (0x03 << 4);
            
            // Определяем позицию блока на странице
            if (spi_block_num > 22)
            {
                Nop();
                // Определяем страницу, на которой находится искомый блок
                page_num_read += (spi_block_num - 1) / 22;
                Nop();
                spi_sec_num = (spi_block_num - 1) % 22 + 1;
                Nop();
            }
            else
            {
                spi_sec_num = spi_block_num;
                Nop();
            }
			llmdb_ans[1] = llmdb_rq[4];
            llmdb_ans[2] = llmdb_rq[5];
            llmdb_ans[3] = llmdb_rq[6];
            llmdb_ans[4] = llmdb_rq[7];
		//	SPI
            Nop();
			SpiReadMem(page_num_read);
            Nop();
		//
			count = (spi_sec_num - 1) * 24 - 2;
            Nop();
			for (a = 2; a < 27; a++)
				llmdb_ans[a + 3] = spi_buf_read[count + a];
            Nop();
		}
		else
		{
			ccs |= (0x0e << 4);
			llmdb_ans[0] = ccs;
			for (a = 1; a < 26; a++)
				llmdb_ans[a] = 0x00;
		}
		llmdb_ans[0] = ccs;
		spi_session_block_success = 0;
	}

	if(cmd == CMD_SPI_READ_BLOCK)
	{	
		ccs |= (0x03 << 4);
		spi_block_num = ((unsigned long int)llmdb_rq[5] << 24) | ((unsigned long int)llmdb_rq[4] << 16) |
						((unsigned long int)llmdb_rq[3] << 8) | llmdb_rq[2];

		page_num_read = spi_block_num / 22;
		spi_sec_num = spi_block_num % 22;
		llmdb_ans[0] = ccs;
		llmdb_ans[1] = llmdb_rq[2];
		llmdb_ans[2] = llmdb_rq[3];
		llmdb_ans[3] = llmdb_rq[4];
		llmdb_ans[4] = llmdb_rq[5];
	//	SPI
		SpiReadMem(page_num_read);
	//
		count = (spi_sec_num - 1) * 24 - 5;
		for (a = 5; a < 30; a++)
			llmdb_ans[a] = spi_buf_read[count + a];
	}

	if(cmd == CMD_SPI_FSPACE)
	{
		ccs |= (0x03 << 4);
		llmdb_ans[0] = ccs;
		RefreshMeta();
		for (a = 6; a < 523; a+=6)
		{
			if ((spi_meta[a] == 0x00) && (spi_meta[a+1] == 0x00))
			{
                spi_free_space = Spi_FSpace(0x00, 0x00);
				if (a > 6)
                {
                    for (g = a-2; g > 9; g-=6)
                    {
                        if (spi_meta[g] != 0xFF)
                        {
                            Nop();
                            spi_free_space = Spi_FSpace(spi_meta[g], spi_meta[g + 1]);
                            Nop();
                            break;
                        }
                    }
                }
				break;
			}
		}
		llmdb_ans[4] = spi_free_space >> 24;
		llmdb_ans[3] = spi_free_space >> 16;
		llmdb_ans[2] = spi_free_space >> 8;
		llmdb_ans[1] = spi_free_space;
	}

	if(cmd == CMD_SPI_READ)
	{	
		page_num_read = ((int)llmdb_rq[2] << 8) | ((int)llmdb_rq[3]);	
		spi_sec_num = llmdb_rq[4];	
		ccs |= (0x03 << 4);
		llmdb_ans[1] = spi_sec_num;
//	SPI
		SpiReadMem(page_num_read);
//
		count = (spi_sec_num - 1) * 24 - 2;
		for (a = 2; a < 27; a++)
			llmdb_ans[a] = spi_buf_read[count + a];

		llmdb_ans[0] = ccs;

	}

	if(cmd == CMD_BINR_START)
	{
		ccs |= (0x00 << 4);
		llmdb_ans[0] = ccs;		
		BINR_start_flag = 1;
	}

	if(cmd == CMD_BINR_STOP)
	{
		ccs |= (0x00 << 4);
		llmdb_ans[0] = ccs;		
		BINR_stop_flag = 1;
	}

	if(cmd == CMD_BINR_GETSTATE)
	{	
		llmdb_sec_num = ((int)llmdb_rq[2]) | ((int)llmdb_rq[3] << 8);	
		if (llmdb_sec_num < 4 && llmdb_sec_num > 0)
		{
				ccs |= (0x03 << 4);
				llmdb_ans[1] = llmdb_rq[2];
                llmdb_ans[2] = llmdb_rq[3];
				count = (llmdb_sec_num - 1) * 24;
//				for (a = 2; a < 26; a++)
//					llmdb_ans[a] = BINR_state[count + a];
                
                if (!binr_state_act_buf)    // если запись идет в массив 1, то читать надо массив 2
                    for (a = 0; a < 26; a++)
                        llmdb_ans[a + 3] = BINR_state_2[count + a];
                else
                    for (a = 0; a < 26; a++)
                        llmdb_ans[a + 3] = BINR_state_1[count + a];
		}
		else
			ccs |= (0x0d << 4);
		llmdb_ans[0] = ccs;

	}
    
    if(cmd == CMD_BINR_GET_FULL_STATE)
	{	
        ccs |= (0x03 << 4);

        if (!binr_state_act_buf)    // если запись идет в массив 1, то читать надо массив 2
            for (a = 0; a < 72; a++)
                llmdb_ans[a + 1] = BINR_state_2[a];
        else
            for (a = 0; a < 72; a++)
                llmdb_ans[a + 1] = BINR_state_1[a];

		llmdb_ans[0] = ccs;

	}
		
	// Расчет контрольной суммы исходящего пакета (если нужен)
	makeCRC_ans();

	// Отправка пакета
	RCSTA2bits.CREN = 0 ;
	LATA |=  (1<<RS485DE) ;	//макс485 переключение направления передачи данных

	for(a=0; a<ans_size; a++)
	{
		TXREG2 = llmdb_ans[a];	
		Nop();	
		while(!TXSTA2bits.TRMT);
	}

	LATA &= ~(1<<RS485DE) ;	//макс485 
	RCSTA2bits.CREN = 1 ;

	addr_done = 0;
	cmd_checked = 0;
	addr_rq = 0;
    for (a = 0; a < 20; a++)
		llmdb_rq[a] = 0x00;

//	flag_send_to_llmdb = 0;
}

void clearBuffer_rq()
{
	rq_size = 0;
	rq_ptr = 0;
}
//---------------------------------------------------------------------------------------------
char checkCRC_rq()
{
	int a;
	char c, ccs;

	c = llmdb_rq[0];
	for(a=1; a<rq_size-1; a++)
		c = c ^ llmdb_rq[a];
	ccs = llmdb_rq[rq_size-1];
	if(ccs == c)
		return 1;
	else
		return 0;
}
//---------------------------------------------------------------------------------------------
void makeCRC_ans()	// ????????? ? ?????????? CRC ? ?????
{
	int a;
	char ccs;
	if(ans_size < 3)
		return;

	ccs = llmdb_ans[0];
	for(a=1; a<ans_size-1; a++)
		ccs = ccs ^ llmdb_ans[a];
	llmdb_ans[ans_size-1] = ccs;
}
