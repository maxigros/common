#ifndef LIFE_H
#define LIFE_H

#include <QTimer>
#include <iostream>
#include <QObject>
#include <QFile>
using namespace std;

class life : public QObject
{
    Q_OBJECT
public:
    explicit life();
            ~life();

private:

    typedef struct
    {
        char state;
    } cell;

    char dead = '-';
    char live = '#';
    char test = '^';

    const static int k = 20;
    const static int l = 40;
    const static int m = k + 2;
    const static int n = l + 2;

    cell this_gen[m][n];
    cell next_gen[m][n];

    void print_gen();
    void update_gen();
    void set_glider();
    void read_file();

    int live_neighbours;

    QTimer tim;

private slots:
    void tim_timeout();
};

#endif // LIFE_H
