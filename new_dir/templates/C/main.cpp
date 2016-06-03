#include <stdio.h>
#include <math.h>
#include <skirl.h>

//----------------------------------------------------------------
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    int w, h;
    Quad** ar = readArray("rli76_angles_fine.dmp", &w, &h);
    if(ar)
        printf("File contains %d lines, %d samples each\n", h, w);
    else
    {
        printf("Unable to read file\n");
        return 0;
    }

    for(int a=0; a<h; a++)
    {
        for(int b=0; b<w; b++)
        {
            float amp = sqrt(ar[a][b].re*ar[a][b].re + ar[a][b].im*ar[a][b].im);
            if(amp<50)
            {
                ar[a][b].re = 0;
                ar[b][b].im = 0;
            }
        }
    }

    writeArray(ar, "rli76_angles_fine_1.dmp", w, h);

    clearArray(ar, h);
    return 0;
}
