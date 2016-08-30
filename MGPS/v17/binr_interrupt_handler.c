#include "defines.h"
#include "globals.h"
#include "spi.h"
/*******************************************************************************
 *  VARIABLES
 * *****************************************************************************
 */
char flag_DLE = 0;	
unsigned char BINR_prev = 0; 
int BINR_bytes = 0; 
int timer_val = 30000;


//#pragma code low_vector = 0x18
#pragma interrupt low_isr
	
void low_isr (void)
{
	char c;		
    int b;	
    unsigned char s;	
    
    if (INTCONbits.TMR0IF)
    {
        INTCONbits.TMR0IF = 0;
        
//        CloseTimer0();
        T0CONbits.TMR0ON = 0;
        
        binr_is_active = 0;
        binr_timer_is_active = 0;
    }
    
	if (PIR1bits.RC1IF) 	
	{	
		PIR1bits.RC1IF = 0;
        
		c = RCREG1 ;
		if(RCSTA1bits.OERR) 
		{  
			if (BINR_cmd_inUse)  
			{
				switch (BINR_cmd_inUse)
				{
					case 0x88:
                        if (!binr_state_act_buf)
                            for (b = 0; b < 90; b++)
                                BINR_state_1[b] = 0x00;
                        else
                            for (b = 0; b < 90; b++)
                                BINR_state_2[b] = 0x00;
						break;

				}
			}
			BINR_cmd_inUse = 0x00;
             
            
			RCSTA1bits.CREN = 0 ;
			RCSTA1bits.CREN = 1 ;
			return ;
		}
		BINR_bytes++;
		if (SPI_active_flag)   
		{
            if (!flag_spi_need_push)
            {
                if (push_saved)
                {
                    push_saved = 0;
                    for (b = 0; b < symbol_pos_saved; b++)
                    {
                        if (!act_buf)
                        {
                            //	write_byte_toBuf(symbol_pos, c);    в первый буфер
                            SPI_CS = 0; 		// CSN high
                            SPI_WR(0x84);
                            SPI_WR(0);
                            SPI_WR(symbol_pos >> 8);
                            SPI_WR(symbol_pos);
                            SPI_WR(c);
                            SPI_CS = 1; 		// CSN low
                        }
                        else
                        {
                            //	write_byte_toBuf(symbol_pos, c);    во второй буфер
                            SPI_CS = 0; 		// CSN high
                            SPI_WR(0x87);
                            SPI_WR(0);
                            SPI_WR(symbol_pos >> 8);
                            SPI_WR(symbol_pos);
                            SPI_WR(c);
                            SPI_CS = 1; 		// CSN low
                        }
                        symbol_pos++;
                    }
                    symbol_pos_saved = 0;
                }
                if (!act_buf)
                {
                    //	write_byte_toBuf(symbol_pos, c);    в первый буфер
                    SPI_CS = 0; 		// CSN high
                    SPI_WR(0x84);
                    SPI_WR(0);
                    SPI_WR(symbol_pos >> 8);
                    SPI_WR(symbol_pos);
                    SPI_WR(c);
                    SPI_CS = 1; 		// CSN low
                }
                else
                {
                    //	write_byte_toBuf(symbol_pos, c);    во второй буфер
                    SPI_CS = 0; 		// CSN high
                    SPI_WR(0x87);
                    SPI_WR(0);
                    SPI_WR(symbol_pos >> 8);
                    SPI_WR(symbol_pos);
                    SPI_WR(c);
                    SPI_CS = 1; 		// CSN low
                }

                symbol_pos++;
                if (symbol_pos == SPI_PAGE_SIZE)
                {
                    flag_spi_need_push = 1;

                    if (act_buf)
                        act_buf = 0;
                    else
                        act_buf = 1;

                    symbol_pos = 0;
                } 
            }
            else
            {
                saved[symbol_pos_saved] = c;
                symbol_pos_saved++;
            }
        }
		
		if (flag_DLE)
		{
			if (BINR_cmd_inUse == 0x88)
			{
				if ((BINR_prev == DLE) && (c==DLE))
				{			
					BINR_bytes--;
					BINR_prev = 0x00;
					return;
				}
			}
			
			if (BINR_bytes == 2)
				BINR_answer = c;

			BINR_cmd_inUse = BINR_answer;
			switch (BINR_answer)
			{
				case 0x88:
                    if (BINR_bytes > 2)
                        if (!binr_state_act_buf)
                            BINR_state_1[BINR_bytes - 3] = c;
                        else
                            BINR_state_2[BINR_bytes - 3] = c;
					break;
			}
			if ( (c == ETX) && (BINR_prev == DLE) )
			{
//                nv08_no_answer = 0;
                // Timer operations
                if (binr_timer_is_active)
                {
//                    WriteTimer0(timer_val);
                    TMR0L = timer_val & 0xFF;
                    TMR0H = (timer_val) >> 8 & 0xFF;
                }
                else
                {
                    binr_timer_is_active = 1;
                    binr_is_active = 1;
//                    OpenTimer0(		
//                        TIMER_INT_ON &
//                        T0_16BIT &
//                        T0_SOURCE_INT &
//                        T0_PS_1_256
//                    );
                    T0CONbits.TMR0ON = 1;
                    T0CONbits.T08BIT = 0;
                    T0CONbits.T0CS = 0;
                    T0CONbits.T0SE = 1;
                    T0CONbits.PSA = 0;
                    T0CONbits.T0PS2 = 1;
                    T0CONbits.T0PS1 = 1;
                    T0CONbits.T0PS0 = 1;
                    
//                    WriteTimer0(timer_val);
                    TMR0L = timer_val & 0xFF;
                    TMR0H = (timer_val >> 8) & 0xFF;
                }
        
        
        
				//flag_have_data = 1;
                if (BINR_answer == 0x88)
                {
                    if (binr_state_act_buf)
                        binr_state_act_buf = 0;
                    else 
                        binr_state_act_buf = 1;
                }
				flag_DLE = 0;
				BINR_bytes = 0;
				BINR_answer = 0x00;
				BINR_prev = 0x00;
				BINR_cmd_inUse = 0x00;
                
				PORTAbits.RA0 = ~PORTAbits.RA0;
			}
			BINR_prev = c;
		}
		else
		{
			if (c == DLE)
				flag_DLE = 1;
			else
				BINR_bytes = 0;
		}
        
	}
}
#pragma code

#pragma code InterruptVectorLow = 0x18             //  This function simply jumps to the ISR code shown above.
void InterruptVectorLow (void)
{
    _asm
    goto low_isr //jump to interrupt routine
    _endasm
}
#pragma code 
