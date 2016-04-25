#include "cp9b_lib.h"
#include <iostream>
using namespace std;

cp9b_lib::cp9b_lib(QObject *parent) :
    QObject(parent)
{
    connect(&serial, SIGNAL(readyRead()), this, SLOT(read_COM()));
}

cp9b_lib::~cp9b_lib()
{
    if (serial.isOpen())
        serial.close();
}

void cp9b_lib::connect_to_COM(QString serialport_name)
{
    serial.setPortName(serialport_name);
    serial.setBaudRate(QSerialPort::Baud115200);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    if (serial.open(QSerialPort::ReadWrite))
    {
        emit send_to_gui(KEY_MSG_FROM_CLI, QString("Port " + serialport_name + " is opened"), 0, 0);
    }
    else
    {
        QString str = serial.errorString();
        emit send_to_gui(KEY_MSG_FROM_CLI, QString("Error at port " + serialport_name + ": " + str), 0, 0);
    }
}

void cp9b_lib::read_COM()
{
    int s = serial.bytesAvailable();
    char* buf = new char[50];
    serial.read(buf, s);
    emit send_to_gui(KEY_MSG_FROM_DEV, 0, buf, s);
    delete buf;
}

void cp9b_lib::send_to_COM(char *data, int data_size)
{
    serial.write((const char*)data, data_size);
}

