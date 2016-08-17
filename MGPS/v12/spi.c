#include "spi.h"

char BINR_cmd_f4_str []	= {	DLE, 0xf4, 0x14, DLE, ETX};



//unsigned char SPI_WR(unsigned char byte)
//{
//	SSPBUF = byte; 
//	while(!SSPSTATbits.BF);
//	return SSPBUF;
//}

void SPI_Init()
{
    SSPSTATbits.SMP = 0;	// input data sampled at end of data output time
	SSPSTATbits.CKE = 1;	// transmit occurs on transition from idle to active clock state
	SSPCON1bits.CKP = 0;	// idle state for clock is a low level
	SSPCON1bits.SSPM3 = 0; 	// SPI Master Mode, clock = Fosc/16
	SSPCON1bits.SSPM2 = 0;		//
	SSPCON1bits.SSPM1 = 0;		//
	SSPCON1bits.SSPM0 = 0;		//

	SPI_CS = 1;

	SSPCON1bits.SSPEN = 1;		// enable SPI

}

void SpiCpyBufToMem(int n)
{
//    unsigned char status;
    // address maker
    unsigned int b3 = 0x00;
	unsigned int b2 = ( n  << 2 ) & 0xFF;   // !
	unsigned int b1 = ( n  >> 6) & 0xFF;    // !
    
	SPI_CS= 0;            //CSN high	
	SPI_WR(0x83);
	SPI_WR(b1);
	SPI_WR(b2);
	SPI_WR(b3);	
	SPI_CS = 1;           // CSN low
    
	while(!SpiStatus());
}

void SpiWriteMem(int n)
{
    int i;
//    unsigned char status;
    // address maker
    unsigned int b3 = 0x00;
	unsigned int b2 = ( n  << 2) & 0xFF;    // !
	unsigned int b1 = ( n  >> 6) & 0xFF;    // !
    
	SPI_CS= 0;            //CSN low	
	SPI_WR(0x82);
	SPI_WR(b1);
	SPI_WR(b2);
	SPI_WR(b3);	
	for (i = 0; i < 528; i++)
    {
		SPI_WR(spi_buf_toWrite[i]); 
    }
	SPI_CS = 1;                    // CSN high
//	while(!SpiStatus());
}

void SpiWriteMem_Meta()
{
    int i;
    
	SPI_CS= 0;            //CSN low	
	SPI_WR(0x85);
	SPI_WR(0);
	SPI_WR(0);
	SPI_WR(0);	
	for (i = 0; i < 528; i++)
    {
		SPI_WR(spi_meta[i]); 
    }
	SPI_CS = 1;                    // CSN high
	while(!SpiStatus());
}

void SpiReadMem(int n)
{
    int i;
    //unsigned char status;
    // address maker
    unsigned int b3 = 0x00 & 0xFF;
	unsigned int b2 = ( n  << 2) & 0xFF;    // !
	unsigned int b1 = ( n  >> 6) & 0xFF;    // !
//    unsigned int b2 = ( page_num_read  << 2) & 0xFF;    // !
//	unsigned int b1 = ( page_num_read  >> 6) & 0xFF;
    
	SPI_CS= 0;            //CSN high		
	SPI_WR(0xd2);
	SPI_WR(b1);
	SPI_WR(b2);
	SPI_WR(b3);	
	W_Dummy(4);	
	for (i = 0; i < 528; i++)
    {
		SPI_RDWR(0x00, spi_buf_read[i]); 
    }
	SPI_CS = 1;                    // CSN low
}
//----------------------------------------------------------------------------------------------
void RefreshMeta()
{
    int i;
	SpiReadMem(0);
	for (i = 0; i < 528; i++)
		spi_meta[i] = spi_buf_read[i];
}
//----------------------------------------------------------------------------------------------
void UploadMeta()
{
	SpiWriteMem_Meta();
}
//----------------------------------------------------------------------------------------------
unsigned long int Spi_FSpace(unsigned char b1, unsigned char b2)
{
	unsigned long int all_space = 8191;
	unsigned long int busy_space = ((int)b1 << 8) | (int)b2;
	return ((all_space - busy_space)*528);
}
//----------------------------------------------------------------------------------------------
unsigned long int Spi_Session_Size(unsigned char b1)
{
	unsigned long int small = ((int)spi_meta[b1 + 2] << 8) | (int)spi_meta[b1 + 3];
	unsigned long int big = ((int)spi_meta[b1 + 4] << 8) | (int)spi_meta[b1 + 5];
	unsigned long int size = (big - small + 1) * 528;
	return size;
}

unsigned char  SpiStatus()  // если 1, то флешка готова к записи
{
    //unsigned char status;
	unsigned char s;
	SPI_CS = 0;
	SPI_WR(0xD7);
	SPI_RDWR(0x00, s);
	SPI_CS = 1;
	s = (int)s >> 7;
	return s;
}

void start_spi()
{
	//countSPIBeginVars();    // Определяются переменные  spi_meta_pos(позиция в мете для новой сессии)
                            // и                        page_num_rec(страница на флешке, на которую будет произведена следующая запись)
    RefreshMeta();
    // Адрес начала сессии
	spi_session_addr[0] = page_num_rec >> 8;
	spi_session_addr[1] = page_num_rec;
    // Записываем в мету название новой сессии и ее начальный адрес
    meta_pos_reserved_1 = spi_meta_pos;
    
	spi_meta[spi_meta_pos] = spi_session_num[0];
	spi_meta[spi_meta_pos + 1] = spi_session_num[1];
    spi_meta[spi_meta_pos + 2] = spi_session_addr[0];
	spi_meta[spi_meta_pos + 3] = spi_session_addr[1];
    UploadMeta();
    
    // Ставим на прием вектора состояния массив 1
    binr_state_act_buf = 0;
    // Запись будет идти в буфер 1 (SPI)
    act_buf = 0;
    // Включаем в обработчике прерывания запись байтов, пришедших по BINR
	SPI_active_flag = 1;
    // Запрос эфемеридов
	SendBinr(BINR_cmd_f4_str, 5);
	delay(1000);
    
    SPI_session_in_process = 1;
}
void stop_spi()
{
    int i;
    SPI_session_in_process = 0;
    // Выключаем в обработчике прерывания запись байтов, пришедших по BINR
	SPI_active_flag = 0;
    
    meta_pos_reserved_2 = spi_meta_pos;
    // Записываем данные из буфера в постоянную память
//	if (!act_buf)
//        // если в данный момент запись идет в буфер А, то "пушить" надо буфер А
//        {
//            for (i = 0; i < 528; i++)
//            {
//                spi_buf_toWrite[i] = spi_buf_receiving_A[i];
//                spi_buf_receiving_A[i] = 0;
//            }
//        }
//        else
//        {
//            for (i = 0; i < 528; i++)
//            {
//                spi_buf_toWrite[i] = spi_buf_receiving_B[i];
//                spi_buf_receiving_B[i] = 0;
//            }
//        }
//    SpiWriteMem(page_num_rec);
    if (!act_buf)
    // если в данный момент запись идет в буфер А, то "пушить" надо буфер А
    {
        
        //	write_byte_toBuf(symbol_pos, c);    в первый буфер
                SPI_CS = 0; 		// CSN high
                SPI_WR(0x84);
                SPI_WR(0);
                SPI_WR(symbol_pos >> 8);
                SPI_WR(symbol_pos);
                for (i = symbol_pos; i < 528; i++)
                    SPI_WR(0);
                SPI_CS = 1; 		// CSN low
                
                Nop();
                
                
                
        SPI_CS= 0;            //CSN high	
        SPI_WR(0x83);
        SPI_WR(( page_num_rec  >> 6) & 0xFF);
        SPI_WR(( page_num_rec  << 2 ) & 0xFF);
        SPI_WR(0x00);	
        SPI_CS = 1;           // CSN low
    }
    else
    {
        //	write_byte_toBuf(symbol_pos, c);    во второй буфер
                SPI_CS = 0; 		// CSN high
                SPI_WR(0x87);
                SPI_WR(0);
                SPI_WR(symbol_pos >> 8);
                SPI_WR(symbol_pos);
                for (i = symbol_pos; i < 528; i++)
                    SPI_WR(0);
                SPI_CS = 1; 		// CSN low
        
                Nop();
        
        SPI_CS= 0;            //CSN high	
        SPI_WR(0x86);
        SPI_WR(( page_num_rec  >> 6) & 0xFF);
        SPI_WR(( page_num_rec  << 2 ) & 0xFF);
        SPI_WR(0x00);	
        SPI_CS = 1;           // CSN low
    }
    while(!SpiStatus());
    // Обновляем мету
    RefreshMeta();
    // Адрес окончания сессии
	spi_session_addr[2] = page_num_rec >> 8;
	spi_session_addr[3] = page_num_rec;
    // Записываем адрес окончания сессии в мету
	spi_meta[spi_meta_pos + 4] = spi_session_addr[2];
	spi_meta[spi_meta_pos + 5] = spi_session_addr[3];
    // Без этой задержки запись не происходит, хз почему
    for (i = 0; i < 5000; i++)
        Nop();
    // Загружаем мету
	UploadMeta();
    // Переводим позицию вперед для следующей сессии
	spi_meta_pos += 6;
    
    meta_pos_reserved_3 = spi_meta_pos;
    // Запись следующей сессии будет производиться на следующую страницу
	page_num_rec++;
}
void restart_spi()
{
    stop_spi();
    delay(1000);
    start_spi();
}

void SpiChipErase()
{
	SPI_CS = 0;
	SPI_WR(0xC7);
	SPI_WR(0x94);
	SPI_WR(0x80);
	SPI_WR(0x9A);
	SPI_CS = 1;
//	while(!SpiStatus());
    spi_fat_is_done = 0;
    spi_no_space = 0;
}

void SpiMakeFat()
{
    //  void SpiFormat0page()
    int i;
	for (i = 0; i < 528; i++)
		spi_buf_toWrite[i] = 0xFF;
	for (i = 0; i < 4; i++)
	{
		spi_buf_toWrite[i] = ID[i];
	}
	spi_buf_toWrite[4] = VERSION >> 8;
	spi_buf_toWrite[5] = VERSION;
	for (i= 6; i < 523; i+=6)
	{
		spi_buf_toWrite[i] = 0x00;
		spi_buf_toWrite[i+1] = 0x00;
	}
    
   	SpiWriteMem(0);
	spi_meta_pos = 6;
	page_num_rec = 1;
    symbol_pos = 0;
    act_buf = 1;
    binr_state_act_buf = 0;
    spi_no_space = 0;
    spi_fat_is_done = 1;
}

void W_Dummy(int p)
{
	unsigned char s;
	int k;
	for (k = 0; k < p; k++)
    {
		SPI_RDWR(0x00, s);
    }
}

void countSPIBeginVars()
{
    int g;
    RefreshMeta(); 
	for (g = 6; g < 523; g+=6)
	{
		if ((spi_meta[g] == 0x00) && (spi_meta[g+1] == 0x00))
		{
			spi_meta_pos = g;
			if (g > 6)
				page_num_rec = ((int)(spi_meta[spi_meta_pos - 2] << 8) | (int)spi_meta[spi_meta_pos - 1]) + 1;

			break;
		}
	}
}
