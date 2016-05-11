#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPortInfo>
#include "serial_stuffing.h"
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

    void on_check_echo_mode_toggled(bool checked);

    void on_button_timer_test_clicked();

public slots:
    void receive_from_cli(int key, QString msg, char *data, int data_size);

};

#endif // MAINWINDOW_H
