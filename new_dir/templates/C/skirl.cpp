#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "skirl.h"

Quad** readArray(const char* path, int* w, int* h)
{
    FILE* F = fopen(path, "rb");
    if(!F)
        return NULL;
    // Первый проход по файлу - определение размера массива
    struct
    {
        int flags;
        int size;
        short int angle;
        short int reserve;
        int time;
    } header;
    int nLines = 0;
    while(!feof(F))
    {
        int res = fread(&header, sizeof(header), 1, F);
        if(res == 0)
            break;
        nLines += 1;
        fseek(F, 4*header.size, SEEK_CUR);  // Переход к след. строке
    }
    Quad** out = (Quad**)malloc(nLines*sizeof(Quad*));
    // Второй проход - чтение данных
    fseek(F, 0, SEEK_SET);
    int y=0;
    while(!feof(F))
    {
        int res = fread(&header, sizeof(header), 1, F);
        if(res == 0)
            break;
        out[y] = (Quad*)malloc(header.size*sizeof(Quad));
        for(int a=0; a<header.size; a++)
        {
            fread(&out[y][a], sizeof(Quad), 1, F);
        }
        y+=1;
    }
    fclose(F);
    *w=header.size;
    *h=nLines;
    return out;
}
//----------------------------------------------------------------
int writeArray(Quad** ar, const char* path, int w, int h)
{
    FILE* F=fopen(path, "wb");
    if(!F)
        return -1;
    struct
    {
        int flags;
        int size;
        short int angle;
        short int reserve;
        int time;
    } header;
    for(int y=0; y<h; y++)
    {
        header.flags = 0;
        header.size = w;
        header.angle=floor(y*10000.0/2400.0);
        header.reserve = 0xDEAD;
        header.time = y;
        fwrite(&header, sizeof(header), 1, F);
        for(int x=0; x<w; x++)
        {
            fwrite(&ar[y][x], sizeof(Quad), 1, F);
        }
    }
    fclose(F);
    return 0;
}
//----------------------------------------------------------------
// Освобождает память, выделенную для массива.
// Параметры:
// ar — указатель на массив
// h — количество строк в массиве
void clearArray(Quad** ar, int h)
{
    for(int a=0; a<h; a++)
        free(ar[a]);
    free(ar);
}
