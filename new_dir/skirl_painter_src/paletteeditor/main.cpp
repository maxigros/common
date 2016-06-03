#include <QtGui/QApplication>
#include "paletteeditorwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PaletteEdtorWindow w;
    w.show();

    return a.exec();
}
