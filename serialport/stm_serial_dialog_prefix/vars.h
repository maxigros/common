#include <stm32l1xx.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_gpio.h>
#include <stm32l1xx_usart.h>
#include <misc.h>

#define BUF_SIZZE   5000
#define PREFIX_BYTE     0xAA
#define PREFIX_SIZE     2

extern unsigned char rx_buf[BUF_SIZZE + PREFIX_SIZE + 2];
extern unsigned char tx_buf[BUF_SIZZE + PREFIX_SIZE + 2];
extern unsigned char prefix_buf[PREFIX_SIZE];
extern int prefix_counter;
extern int rx_count;
extern int tx_count;
extern unsigned char previous_byte;
extern char flag_got_prefix;
extern unsigned short int package_counter;
extern char pack_correct;
extern unsigned short int pack_size;

extern GPIO_InitTypeDef port;
extern USART_InitTypeDef usart;
extern NVIC_InitTypeDef nvic;



#define BAUDRATE 		115200
#define BUTTON 			GPIO_Pin_0
#define LED_BLUE 		GPIO_Pin_6
#define LED_GREEN 	    GPIO_Pin_7





// COMMANDS
#define CMD_HELLO           0x55
#define CMD_COUNTER         0xCC
#define CMD_COUNTER_ZERO    0xFF