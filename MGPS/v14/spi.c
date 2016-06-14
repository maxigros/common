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
    unsigned char b3 = 0x00;
	unsigned char b2 = ( n  << 2 ) & 0xFF;   // !
	unsigned char b1 = ( n  >> 6) & 0xFF;    // !
    
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
    unsigned char b3 = 0x00;
	unsigned char b2 = ( n  << 2) & 0xFF;    // !
	unsigned char b1 = ( n  >> 6) & 0xFF;    // !
    
	SPI_CS= 0;            //CSN low	
	SPI_WR(0x82);
	SPI_WR(b1);
	SPI_WR(b2);
	SPI_WR(b3);	
	for (i = 0; i < SPI_PAGE_SIZE; i++)
    {
		SPI_WR(spi_meta[i]); 
    }
	SPI_CS = 1;                    // CSN high
	while(!SpiStatus());
}

void SpiWriteMem_Meta(char buf)
{
    int i;
    unsigned char b3 = 0x00;
	unsigned char b2 = ( SPI_PAGES_NUM  << 2) & 0xFF;    // !
	unsigned char b1 = ( SPI_PAGES_NUM  >> 6) & 0xFF;    // !
    
    // zero page
	SPI_CS= 0;            //CSN low	
    if (!buf) {
        SPI_WR(0x82);
    }
    else {
        SPI_WR(0x85);
    }
	SPI_WR(0);
	SPI_WR(0);
	SPI_WR(0);	
	for (i = 0; i < SPI_PAGE_SIZE; i++)
    {
		SPI_WR(spi_meta[i]); 
    }
	SPI_CS = 1;                    // CSN high
	while(!SpiStatus());
    
    // last page
//    
//    SPI_CS= 0;            //CSN low	
//	SPI_WR(0x85);
//	SPI_WR(b1);
//	SPI_WR(b2);
//	SPI_WR(b3);	
//	for (i = 0; i < SPI_PAGE_SIZE; i++)
//    {
//		SPI_WR(spi_meta[i]); 
//    }
//	SPI_CS = 1;                    // CSN high
//	while(!SpiStatus());
}

void SpiReadMem(int n)
{
    int i;
    // address maker
    unsigned char b3 = 0x00 & 0xFF;
	unsigned char b2 = ( n  << 2) & 0xFF;    // !
	unsigned char b1 = ( n  >> 6) & 0xFF;    // !

    
	SPI_CS= 0;            //CSN high		
	SPI_WR(0xd2);
	SPI_WR(b1);
	SPI_WR(b2);
	SPI_WR(b3);	
	W_Dummy(4);	
	for (i = 0; i < SPI_PAGE_SIZE; i++)
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
    // TODO: сравнение старта буфера с mgps
	for (i = 0; i < SPI_PAGE_SIZE; i++)
		spi_meta[i] = spi_buf_read[i];
}
//----------------------------------------------------------------------------------------------

void UploadMeta(char buf)
{
	SpiWriteMem_Meta(buf);
    //SpiWriteMem(8189);
}
//----------------------------------------------------------------------------------------------
unsigned long int Spi_FSpace(unsigned char b1, unsigned char b2)
{
	unsigned long int all_space = SPI_PAGES_NUM - 1;
	unsigned long int busy_space = ((int)b1 << 8) | (int)b2;
	return ((all_space - busy_space)*SPI_PAGE_SIZE);
}
//----------------------------------------------------------------------------------------------
unsigned long int Spi_Session_Size(int b1)
{
    unsigned long int small = 0;
    unsigned long int big = 0;
    unsigned long int size = 0;
    Nop();
	small = ((int)spi_meta[b1 + 2] << 8) | (int)spi_meta[b1 + 3];
	big = ((int)spi_meta[b1 + 4] << 8) | (int)spi_meta[b1 + 5];
	size = (big - small + 1) * SPI_PAGE_SIZE;
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
    //RefreshMeta();
    // Адрес начала сессии
	spi_session_addr[0] = page_num_rec >> 8;
	spi_session_addr[1] = page_num_rec;
    // Записываем в мету название новой сессии и ее начальный адрес
    
	spi_meta[spi_meta_pos] = spi_session_num[0];
	spi_meta[spi_meta_pos + 1] = spi_session_num[1];
    spi_meta[spi_meta_pos + 2] = spi_session_addr[0];
	spi_meta[spi_meta_pos + 3] = spi_session_addr[1];
    
    
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
    
    if (!act_buf)
    // если в данный момент запись идет в буфер А, то "пушить" надо буфер А
    {
        
        //	write_byte_toBuf(symbol_pos, c);    в первый буфер
                SPI_CS = 0; 		// CSN high
                SPI_WR(0x84);
                SPI_WR(0);
                SPI_WR(symbol_pos >> 8);
                SPI_WR(symbol_pos);
                for (i = symbol_pos; i < SPI_PAGE_SIZE; i++)
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
                for (i = symbol_pos; i < SPI_PAGE_SIZE; i++)
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
    // ckeck meta BEFORE read - memory corrupt
    //RefreshMeta();
    // ckeck meta AFTER read
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
    if (!act_buf)   //если при окончании сессии использовался 1 буфер, то тут исп 2
        UploadMeta(1);
    else
        UploadMeta(0);
    // Переводим позицию вперед для следующей сессии
	spi_meta_pos += 6;
    
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
    int i;
    
	SPI_CS = 0;
	SPI_WR(0xC7);
	SPI_WR(0x94);
	SPI_WR(0x80);
	SPI_WR(0x9A);
	SPI_CS = 1;
//	while(!SpiStatus());
    spi_fat_is_done = 0;
    spi_few_space = 0;
    
    for (i = 0; i < SPI_PAGE_SIZE; i++)
		spi_meta[i] = 0x00;
}

void SpiMakeFat()
{
        int i;
	for (i = 0; i < SPI_PAGE_SIZE; i++)
		spi_meta[i] = 0xFF;
	for (i = 0; i < 4; i++)
	{
		spi_meta[i] = ID[i];
	}
	spi_meta[4] = VERSION >> 8;
	spi_meta[5] = VERSION;
	for (i= 6; i < 523; i+=6)
	{
		spi_meta[i] = 0x00;
		spi_meta[i+1] = 0x00;
	}
    
    UploadMeta(0);
    
    
//   	SpiWriteMem(0);
	spi_meta_pos = 6;
	page_num_rec = 1;
    symbol_pos = 0;
    act_buf = 1;
    binr_state_act_buf = 0;
    spi_few_space = 0;
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
