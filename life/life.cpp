#include "life.h"

life::life()
{
    connect(&tim, SIGNAL(timeout()), this, SLOT(tim_timeout()));

    // dead generation
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
        {
            this_gen[i][j].state = dead;
            next_gen[i][j].state = dead;
        }

    // first generation
    set_glider();
//    set_test();

    update_gen();
    print_gen();

//    exit(EXIT_SUCCESS);
    tim.start(200);
}

life::~life()
{

}

void life::print_gen()
{
    // весь вывод
//    for (int i = 0; i < m; i++)
//    {
//        for (int j = 0; j < n; j++)
//            cout << this_gen[i][j].state;
//        cout << endl;
//    }
//    cout << endl;

    // центральная часть
    for (int i = 1; i < m - 1; i++)
    {
        for (int j = 1; j < n - 1; j++)
            cout << this_gen[i][j].state;
        cout << endl;
    }
    cout << endl;

}

void life::update_gen()
{
    // центральная часть
    for (int i = 1; i < m - 1; i++)
        for (int j = 1; j < n - 1; j++)
            next_gen[i][j].state = next_gen[i][j].state;

    // левая кромка
    for (int i = 1; i < m - 1; i++)
        next_gen[i][0].state = next_gen[i][n - 2].state;

    // правая кромка
    for (int i = 1; i < m - 1; i++)
        next_gen[i][n - 1].state = next_gen[i][1].state;

    // нижняя кромка
    for (int j = 0; j < n; j++)
        next_gen[m - 1][j].state = next_gen[1][j].state;

    // верхняя кромка
    for (int j = 0; j < n; j++)
        next_gen[0][j].state = next_gen[m - 2][j].state;

    // копирование
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            this_gen[i][j].state = next_gen[i][j].state;

}

void life::set_glider()
{
    next_gen[1][2].state = live;
    next_gen[2][3].state = live;
    next_gen[3][1].state = live;
    next_gen[3][2].state = live;
    next_gen[3][3].state = live;
}

void life::set_test()
{
    for (int i = 1; i < n - 1; i++)
        next_gen[1][i].state = '^';
    for (int i = 1; i < n - 1; i++)
        next_gen[m - 2][i].state = '*';
    for (int i = 2; i < m - 2; i++)
        next_gen[i][1].state = '$';
    for (int i = 2; i < m - 2; i++)
        next_gen[i][n - 2].state = '%';
}

void life::read_file()
{
    QFile f("first_gen.txt");
    if (f.open(QIODevice::ReadOnly))
    {}
    else
    {}
}

void life::tim_timeout()
{
    for (int i = 1; i < m - 1; i++)
        for (int j = 1; j < n - 1; j++)
        {
            live_neighbours = 0;
            // текущая клетка - живая
            if (this_gen[i][j].state == live)
            {
                for (signed int x = -1; x < 2; x++)
                    for (signed int y = -1; y < 2; y++)
                    {
                        // текущую клетку не рассматриваем
                        if (!((x == 0) && (y == 0)))
                                if (this_gen[i + x][j + y].state == live)
                                    live_neighbours++;
                    }
                if (live_neighbours == 2 || live_neighbours == 3)
                    next_gen[i][j].state = live;
                else
                    next_gen[i][j].state = dead;
            }
            // текущая клетка - мертвая
            else
            {
                for (int x = -1; x < 2; x++)
                    for (int y = - 1; y < 2; y++)
                        // текущую клетку не рассматриваем
                        if (!((x ==0) && (y ==0)))
                                if (this_gen[i + x][j + y].state == live)
                                    live_neighbours++;
                if (live_neighbours == 3)
                    next_gen[i][j].state = live;
                else
                    next_gen[i][j].state = dead;
            }
    }
    update_gen();
    print_gen();
}
