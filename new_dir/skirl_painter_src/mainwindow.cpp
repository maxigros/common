#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    viewWindow = new ViewWindow(0);
    viewWindow->show();

    viewWindow->setMaxFrames(100);
    viewWindow->setAmpLimit(100);
    viewWindow->fieldWidget()->forceRepaint();
    viewWindow->setGenMode(FDM_QUICK);

    IKO_Metrics im;

    im.A_max = 10000;
    im.minRange_LFM = 0;
    im.minRange_SKI = 0;
    im.rangeStep = 0.4;
    viewWindow->setIkoMetrics(im);

    viewWindow->setGrid(15, 30);
    viewWindow->setGridColor(QColor(255,255,0));
}

MainWindow::~MainWindow()
{
    delete ui;
}
