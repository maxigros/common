#include <QCoreApplication>
#include "life.h"

life *d;
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    d = new life();
    return a.exec();
}
