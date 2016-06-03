//#include "mainwindow.h"
#include "viewwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ViewWindow w;
    w.show();

    w.setMaxFrames(2500);
    w.setAmpLimit(150);
    w.setGenMode(FDM_QUICK);

    IKO_Metrics im;
    memset(&im, 0, sizeof(IKO_Metrics));
    im.A_max = 10000;
    im.minRange_LFM = 0;
    im.minRange_SKI = 0;
    im.rangeStep = 0.4;
    im.angleSKO = 2;
    w.setIkoMetrics(im);

    w.setGrid(100, 30);
    w.setGridColor(QColor(255,255,0));

    w.on_mode_comboBox_currentIndexChanged(1);

    return a.exec();
}
