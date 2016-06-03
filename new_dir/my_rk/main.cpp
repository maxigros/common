#include <stdio.h>
#include <math.h>
#include <skirl.h>

#include <iostream>
using namespace std;

//----------------------------------------------------------------

void check_dot(Quad** ar, int a, int b, int *sum);


int main(int argc, char *argv[])
{
    /*
     * Область «Дерево возле забора» расположена в диапазоне от 225 до 260 канала
    дальности и с 310 по 370 канал времени (углы с 1289 по 1539).
     *
     * */

    (void)argc;
    (void)argv;
    int x, y, amplitude, x_max, x_min, y_max, y_min, S;
    S = 1;

    cout << "x = ";
    cin >> x;
    cout << endl;

    cout << "y = ";
    cin >> y;
    cout << endl;

    cout << "Enter amplitude: " << endl;
    cin >> amplitude;

    int w, h;
    Quad** ar = readArray("rli76_angles_fine.dmp", &w, &h);
    if(ar)
        printf("File %s contains %d lines, %d samples each\n", "rli76_angles_fine.dmp", h, w);
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
            if(amp < amplitude)
            {
                ar[a][b].re = 0;
                ar[a][b].im = 0;
                ar[a][b].light = false;
            }
            else
                ar[a][b].light = true;
        }
    }
    writeArray(ar, "rli76_angles_fine_1.dmp", w, h);
    clearArray(ar, h);



    Quad** ar2 = readArray("rli76_angles_fine_1.dmp", &w, &h);
    if(ar2)
        printf("File %s contains %d lines, %d samples each\n", "rli76_angles_fine_1.dmp", h, w);
    else
    {
        printf("Unable to read file\n");
        return 0;
    }

    check_dot(ar2, &x, &y, S);





    return 0;
}

void check_dot(Quad** ar, int a, int b, int *sum)
{
    for (int i = -1; i < 2; i++)
        for (int j = -1; j < 2; j++)
            if (i != j)
            {
                if (ar[a + i][b + j].light == true)
                {
                    sum++;
                    ar[a + i][b + j].light = false;
                    check_dot(ar, &(a + i), &(b + j), sum);
                }
            }
}
