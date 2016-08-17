#include <p18cxxx.h>
#include <usart.h>
#include <string.h>
#include <timers.h>

/*******************************************************************************
 *  LLMDB COMMANDS
 * *****************************************************************************
 */
#define CMD_VERSION         0x01    // Команда на запрос версии ПО, чтение
#define SZRX_VERSION		3		// Размер командного сообщения
#define SZTX_VERSION		4		// Размер ответного сообщения

#define CMD_SPI_READ		0x02	// Команда на запрос блока страницы флешки, чтение
#define SZRX_SPI_READ		6		// Размер командного сообщения
#define SZTX_SPI_READ		27		// Размер ответного сообщения

#define CMD_BINR_GETSTATE		0x03	// Команда на запрос блока вектора состояния, чтение
#define SZRX_BINR_GETSTATE      5       // Размер командного сообщения
#define SZTX_BINR_GETSTATE		28		// Размер ответного сообщения

#define CMD_BINR_GET_FULL_STATE		0x09	// Команда на запрос полного вектора состояния, чтение
#define SZRX_BINR_GET_FULL_STATE    3       // Размер командного сообщения
#define SZTX_BINR_GET_FULL_STATE	75		// Размер ответного сообщения

#define CMD_BINR_START		0x84	// Команда на старт посылок BINR, запись
#define SZRX_BINR_START		3		// Размер командного сообщения
#define SZTX_BINR_START		1		// Размер ответного сообщения

#define CMD_BINR_STOP		0x85	// Команда на прекращение посылок BINR, запись
#define SZRX_BINR_STOP		3		// Размер командного сообщения
#define SZTX_BINR_STOP		1		// Размер ответного сообщения

#define CMD_ID		0x06	// Команда на запрос идентификатора устройства, чтение
#define SZRX_ID		3		// Размер командного сообщения
#define SZTX_ID		6		// Размер ответного сообщения

#define CMD_SERIAL		0x07	// Команда на запрос серийного номера устройства, чтение
#define SZRX_SERIAL     3		// Размер командного сообщения
#define SZTX_SERIAL     4		// Размер ответного сообщения

#define CMD_LLMDB_RATE		0x88	// Команда на установку скорости передачи данных, запись
#define SZRX_LLMDB_RATE		4		// Размер командного сообщения
#define SZTX_LLMDB_RATE		1		// Размер ответного сообщения

#define CMD_LLMDB_TEST		0x55	// Команда на установку скорости передачи данных, запись
#define SZRX_LLMDB_TEST		3		// Размер командного сообщения
#define SZTX_LLMDB_TEST		1		// Размер ответного сообщения

#define CMD_DEV_STATUS		0x14	// Байт статуса устройства, чтение
#define SZRX_DEV_STATUS     3		// Размер командного сообщения
#define SZTX_DEV_STATUS		3		// Размер ответного сообщения 

#define CMD_SPI_START		0x8a	// Команда на старт новой сессии SPI, запись
#define SZRX_SPI_START      5		// Размер командного сообщения
#define SZTX_SPI_START		1		// Размер ответного сообщения 

#define CMD_SPI_STOP		0x8b	// Команда на прекращение сессии SPI, запись
#define SZRX_SPI_STOP		3		// Размер командного сообщения
#define SZTX_SPI_STOP		1		// Размер ответного сообщения

#define CMD_SPI_ERASE		0x8c	// Команда на форматирование флешки, запись
#define SZRX_SPI_ERASE     3		// Размер командного сообщения
#define SZTX_SPI_ERASE      1		// Размер ответного сообщения 

#define CMD_SPI_MAKE_FAT		0x90	// Команда на создание файловой системы на флешке, запись
#define SZRX_SPI_MAKE_FAT      3		// Размер командного сообщения
#define SZTX_SPI_MAKE_FAT      1		// Размер ответного сообщения 

#define CMD_SPI_FSPACE		0x0f		// Команда на запрос свободного места на флешке, чтение
#define SZRX_SPI_FSPACE     3			// Размер командного сообщения
#define SZTX_SPI_FSPACE     6			// Размер ответного сообщения

#define CMD_SPI_SESSION_SIZE		0x0d		// Команда на запрос размера сессии, чтение
#define SZRX_SPI_SESSION_SIZE		5			// Размер командного сообщения
#define SZTX_SPI_SESSION_SIZE		6			// Размер ответного сообщения

#define CMD_SPI_SESSION_BLOCK		0x11		// Команда на запрос блока сессии, чтение
#define SZRX_SPI_SESSION_BLOCK		9			// Размер командного сообщения
#define SZTX_SPI_SESSION_BLOCK		30			// Размер ответного сообщения

#define CMD_SPI_READ_BLOCK		0x0e	// Команда на запрос произвольного блока флешки
#define SZRX_SPI_READ_BLOCK		7		// Размер командного сообщения 
#define SZTX_SPI_READ_BLOCK		30		// Размер ответного сообщения 

/*******************************************************************************
 *  SPI
 * *****************************************************************************
 */
 #define SPI_PAGE_SIZE  528
 #define SPI_PAGES_NUM  8192 
 #define SPI_SCK        LATCbits.LATC3        // Clock pin, PORTC pin 3 
 #define SPI_SO        LATCbits.LATC5        // Serial output pin, PORTC pin 5 
 #define SPI_SI        PORTCbits.RC4        // Serial input pin, PORTC pin 4 
 #define SPI_CS        LATCbits.LATC2        // CS output pin, PORTC pin 2
 #define SPI_CE        LATCbits.LATC1        // CE output pin, PORTC pin 1
 #define SPI_IRQ        PORTBbits.RB0        // IRQ input pin, PORTB pin 0
 #define SPI_SCALE      4                      // postscaling of signal unsigned char b1, b2, b3;

/*******************************************************************************
 *  USART
 * *****************************************************************************
 */
// BAUDCON
#define BAUDCON_BRG16   3
// Константы скоростей передачи данных для USART
#define baud_25			3		// 1250000	????? ????????: 25
#define baud_23			4		// 1000000	????? ????????: 23
#define baud_17			42		// 115200	????? ????????: 17
#define baud_16			86		// 57600		????? ????????: 16
#define baud_14			259		// 19200		????? ????????: 14
#define baud_13			520		// 9600		????? ????????: 13


/*******************************************************************************
 *  BINR VARIABLES
 * *****************************************************************************
 */
#define DLE 0x10	// байт разделитель
#define ETX 0x03	// признак конца пакета



/*******************************************************************************
 *  MAX485 AND USART REGISTERS
 * *****************************************************************************
 */

// LATA
#define LED				0
#define RS485REn		2
#define RS485DE			3

// TXSTA
#define TXSTA_TX9D		0
#define TXSTA_TRMT		1
#define TXSTA_BRGH		2
#define TXSTA_SENDB		3
#define TXSTA_SYNC		4
#define TXSTA_TXEN		5
#define TXSTA_TX9		6
#define TXSTA_CSRC		7

// RCSTA
#define RCSTA_RX9D		0
#define RCSTA_OERR		1
#define RCSTA_FERR		2
#define RCSTA_ADDEN		3
#define RCSTA_CREN		4
#define RCSTA_SREN		5
#define RCSTA_RX9		6
#define RCSTA_SPEN		7



