#ifndef COLORPALETTE_H
#define COLORPALETTE_H

#include <stdint.h>

typedef struct
{
    char        ID[2];      // Идентификатор файла, "PF"
    int16_t     version;    // Версия файла
    uint32_t    numColors;  // Количество цветов в палитре
} PaletteHeader;

typedef struct
{
    PaletteHeader   header;
    uint8_t*        data;
} Palette;

// Версии палитры - трехзначное число. Первая цифра - размер элемента палитры
// в байтах
#define PV_2RGB     200 // RGB-цвет, 0 + 5 бит + 5 бит + 5 бит.
#define PV_2RGGB    201 // RGB-цвет, 5 бит + 6 бит + 5 бит

#define PV_3RGB     300 // RGB-цвет, по байту на канал, 3 байта

#define PV_4RGB0    400 // RGB-цвет, по байту на канал, выровненный до 4 байт,
                        // старший байт содержит 0x00
#define PV_4RGBF    401 // RGB-цвет, по байту на канал, выровненный до 4 байт
                        // старший байт содержит 0xFF
#define PV_4RGBA    402 // RGB-цвет с альфа-каналом, по байту на канал, 4 байта

#endif // COLORPALETTE_H
