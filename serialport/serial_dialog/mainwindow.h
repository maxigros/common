#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPortInfo>
#include "serial_stuffing.h"
#include "serial_prefix.h"
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
    serial_stuffing *stuffing_client;
    serial_prefix *prefix_client;

    bool stuffing_mode = true;
    bool prefix_mode = false;

private slots:
    void button_send_clicked();
    void button_connect_clicked();

    void on_button_timer_test_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_pushButton_clicked();

public slots:
    void receive_from_cli(int key, QString msg, char *data, int data_size, int* nums, int nums_size);

};

#endif // MAINWINDOW_H
