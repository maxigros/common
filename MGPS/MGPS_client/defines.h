#ifndef DEFINES_H
#define DEFINES_H

#include <QDebug>
#include <QString>

//***** COMMANDS    *****
#define CMD_VERSION                         0x01    // 1
#define CMD_ID                              0x06    // 6
#define CMD_SERIAL                          0x07    // 7
#define CMD_STATUS                          0x14    // 20
#define CMD_BINR_START                      0x84    // 132
#define CMD_BINR_STOP                       0x85    // 133
#define CMD_STATEVECTOR_BLOCK               0x03    // 3
#define CMD_STATEVECTOR_FULL                0x09    // 9
#define CMD_START_SESSION                   0x8a    // 138
#define CMD_STOP_SESSION                    0x8b    // 139
#define CMD_FLASH_ERASE                     0x8c    // 140
#define CMD_FLASH_MAKE_FAT                  0x90    // 144
#define CMD_FLASH_FREE_SPACE                0x0f    // 15
#define CMD_FLASH_READ_BLOCK                0x0e    // 14
#define CMD_FLASH_READ_SESSION_BLOCK        0x11    // 17
#define CMD_FLASH_SESSION_SIZE              0x0d    // 13
//***********************

//***** KEYS    *****
#define KEY_STATUS              1
#define KEY_SESSION_SIZE        2
#define KEY_FLASH_FREE_SPACE    3
#define KEY_MODE_CHANGED        4
#define KEY_PROGRESS_BAR        5
//*******************

//***** SERVICE COMMANDS    *****
#define SERVICE_CMD_MANUAL_DATA            150
//#define CLIENT_OPEN_SOCKET                 151
//#define CLIENT_CLOSE_SOCKET                152
//*******************************

//****  MODES   *****
#define MODE_NONE               10
#define MODE_SESSIONS_REC       11
#define MODE_SESSION_DOWNLOAD   12
#define MODE_FLASH_DOWNLOAD     13
//*******************

//***** CONFIG  *****

//#define SESSION_DURATION   5000    // msec
//#define LLMDB_TIMEOUT      50      // msec

/*
 * Uncomment to enable asking free space after each session
 * If commented, free space will be asked each 2 sec
 */
//#define ASK_FREE_SPACE_AFTER_SESSION    1

/*
 * Uncomment to enable random duration of sessions (3 to 10 sec)
 * If commented, duration is set from SESSION_DURATION
 */
//#define RANDOM_SESSION_DURATION     1

//*******************

typedef struct
{
    unsigned char dev_addr = 0;
    unsigned char cmd = 0;
    QString str = 0;
    int num = 0;
} cmd_data;

#endif // DEFINES_H
