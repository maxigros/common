#include "defines.h"
#include "globals.h"

/*******************************************************************************
 *  EXTERN VARIABLES
 * *****************************************************************************
 */
extern unsigned char addr_rq;
extern int rq_ptr;
extern char llmdb_oerr_flag;
extern unsigned char llmdb_rq[20];

//#pragma code high_vector = 0x08
#pragma interrupt high_isr
void high_isr (void)
{
    if (PIR3bits.RC2IF)  	 		//?????????? ?? ???????? ?????? LLMDB
	{
		if (RCSTA2bits.RX9D)
		{
			RCSTA2bits.RX9D = 0;
			addr_rq = RCREG2;
			rq_ptr = 1;
			return;;
		}
		if(RCSTA2bits.OERR)	
		{	
			llmdb_oerr_flag = 1;	
			return ;	
		}	
		llmdb_rq[rq_ptr] = RCREG2; 	
		rq_ptr++ ;
	}
}
#pragma code

#pragma code InterruptVectorHigh = 0x08             //  This function simply jumps to the ISR code shown above.
void InterruptVectorHigh (void)
{
    _asm
    goto high_isr //jump to interrupt routine
    _endasm
}
#pragma code 
