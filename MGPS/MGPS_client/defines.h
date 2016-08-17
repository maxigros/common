#ifndef DEFINES_H
#define DEFINES_H

#include <QDebug>

//***** COMMANDS    *****
#define CMD_VERSION                         0x01
#define CMD_ID                              0x06
#define CMD_SERIAL                          0x07
#define CMD_STATUS                          0x14
#define CMD_BINR_START                      0x84
#define CMD_BINR_STOP                       0x85
#define CMD_STATEVECTOR_BLOCK               0x03
#define CMD_STATEVECTOR_FULL                0x09
#define CMD_START_SESSION                   0x8a
#define CMD_STOP_SESSION                    0x8b
#define CMD_FLASH_ERASE                     0x8c
#define CMD_FLASH_MAKE_FAT                  0x90
#define CMD_FLASH_FREE_SPACE                0x0f
#define CMD_FLASH_READ_BLOCK                0x0e
#define CMD_FLASH_READ_SESSION_BLOCK        0x11
#define CMD_FLASH_SESSION_SIZE              0x0d
//***********************

//***** SERVICE COMMANDS    *****
#define CLIENT_OPEN_SOCKET       1001
#define CLIENT_CLOSE_SOCKET      1002
//*******************************

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
    char dev_addr;
    char cmd;
    char* data;
    int data_size;
} struct_cmd;

#endif // DEFINES_H
