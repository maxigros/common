#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPortInfo>
#include "cp9b_lib.h"
#include <iostream>
using namespace std;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    cp9b_lib *cp9_client;

private slots:
    void button_send_clicked();
    void button_connect_clicked();

public slots:
    void receive_from_cli(int key, QString msg, char *data, int data_size);

};

#endif // MAINWINDOW_H
