#include "binr.h"

char BINR_rq_state [] = {	DLE, 0x27, 0x01, DLE, ETX};
char BINR_stop []		= {	DLE, 0x0e, DLE, ETX };
char BINR_cmd_60 []	= {	DLE, 0x21, 0x01, DLE, ETX};
char BINR_cmd_70 []    = {	DLE, 0x18, DLE, ETX };
char BINR_cmd_d71 [] = {	DLE, 0xd7, 0x02, 0x01, DLE, ETX};
char BINR_cmd_d72 [] = {	DLE, 0xd7, 0x03, 0x01, 0x00, DLE, ETX};
char BINR_cmd_a0 []	= {	DLE, 0xa0, 0x03, 0x01, DLE, ETX};
char BINR_cmd_d5 []	= {	DLE, 0xd5, 0x01, DLE, ETX};
char BINR_cmd_f4 []	= {	DLE, 0xf4, 0x14, DLE, ETX};

char getOddParity(char p)	// ??????? ??? ???????? ??? ?????
{
	p = p ^ (p >> 4 | p << 4);
	p = p ^ (p >> 2);
	p = p ^ (p >> 1);
	return !(p & 1);
}
//---------------------------------------------------------------------------------------------
void SendUsart1(char w)	// ???????? ???? ? USART1
{
	if(!TXSTA1bits.TXEN) 
		return ;
	
	while(!PIR1bits.TX1IF) ;

	TXSTA1bits.TX9D = getOddParity(w) ;
	TXREG1 = w ;

	Nop();

	while(!TXSTA1bits.TRMT) ;
}
//---------------------------------------------------------------------------------------------
void SendBinr (char* str, int size) // ???????? ?????? ? USART1
{
	int a ;
	for(a=0 ; a<size ; a++)
	{
		SendUsart1(str[a]) ;
	}
	Nop();
}
//---------------------------------------------------------------------------------------------
void BINR_start() // ???????? ????????? ??????? ?? NV08
{

	SendBinr (BINR_stop, 4) ;
	delay(1000);
    
	SendBinr(BINR_cmd_d71, 6);
	delay(1000);
	SendBinr(BINR_cmd_d72, 7);
	delay(1000);
	SendBinr(BINR_cmd_a0, 6);
	delay(1000);
	SendBinr(BINR_cmd_d5, 5);
	delay(1000);
	SendBinr(BINR_cmd_f4, 5);
	delay(1000);
    
	SendBinr(BINR_rq_state, 5);
	delay(1000);

//	SendBinr(BINR_cmd_60, 5);
//	delay(1000);
}
