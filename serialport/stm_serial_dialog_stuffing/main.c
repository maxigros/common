#include "vars.h"
#include "functions.h"


int main()
{
    clear_rx_buf();
    
	Init();
    // USART RX interrupts enabled
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    // USART enabled
	USART_Cmd(USART1, ENABLE);
	
	GPIO_SetBits(GPIOB, LED_GREEN);
	
	int button_state;
	char flag = 1;
	while(1)
	{
		button_state = GPIO_ReadInputDataBit(GPIOA, BUTTON);
		if (button_state == 1)
		{
			GPIO_SetBits(GPIOB, LED_BLUE);
			GPIO_ResetBits(GPIOB, LED_GREEN);
			if (flag)
			{
				//Send_USART1(greetings, string_size(greetings));
				flag = 0;
			}
		}
		else
		{
			GPIO_SetBits(GPIOB, LED_GREEN);
			GPIO_ResetBits(GPIOB, LED_BLUE);
			if (!flag)
				flag = 1;
		}
	}
}


