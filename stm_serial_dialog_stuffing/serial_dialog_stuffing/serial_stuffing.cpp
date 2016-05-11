#include "serial_stuffing.h"
#include <iostream>
using namespace std;

cp9b_lib::cp9b_lib(QObject *parent) :
    QObject(parent)
{
    connect(&serial, SIGNAL(readyRead()), this, SLOT(read_COM()));
    connect(&tim, SIGNAL(timeout()), this, SLOT(tim_timeout()));
}

cp9b_lib::~cp9b_lib()
{
    if (serial.isOpen())
        serial.close();
}

void cp9b_lib::extract_packs(QByteArray bb, int* dle_pos)
{
    // обрезаем справа
    QByteArray b = bb.remove(dle_pos[1], bb.size() - dle_pos[1]);
    // обрезаем слева
    b = b.remove(0, dle_pos[0] + 1);

    for (int i = 0; i < b.size(); i++)
        if (b[i] == ESC)
        {
            // инвертирование 5 бита
            if (b[i + 1] & (1 << 5))  // если 5 бит ненулевой
                b[i + 1] = b[i + 1] & (~(1 << 5));
            else
                b[i + 1] = b[i + 1] | (1 << 5);

            b.remove(i, 1);
        }

    char* buf = new char[50];
    memcpy(buf, b.data(), b.count());
    int s = b.count();

    // проверка правильности пришедшего пакета
    if (timer_test_mode)
    {
        packs_received++;
        for (int i = 0; i < s; i++)
            if (buf[i] != test_buf[i])
            {
                packs_received--;
                break;
            }

        if (!tim.isActive())
        {
            timer_test_mode = false;
            emit send_to_gui(KEY_MSG_PACKSNUM,
                             QString("%1 packages sent").arg(packs_sent),
                             0,
                             packs_received);
        }
    }

    emit send_to_gui(KEY_MSG_FROM_DEV, 0, buf, s);
    b.clear();
    delete buf;
}

void cp9b_lib::tim_timeout()
{

    send_to_COM(test_buf, 5);
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
        emit send_to_gui(KEY_MSG_FROM_CLI,
                         QString("Port %1 is opened")
                                    .arg(serialport_name),
                         0,
                         0);
    }
    else
    {
        QString str = serial.errorString();
        emit send_to_gui(KEY_MSG_FROM_CLI,
                         QString("Error at port %1 : %2")
                                    .arg(serialport_name)
                                    .arg(str),
                         0,
                         0);
    }
}

void cp9b_lib::read_COM()
{
    QByteArray buf = serial.readAll();
    big_buf.append(buf);

    bool got_dle = false;
    int* dle_pos = new int[2];
    int count = 0;

//    cout << QString(big_buf.toHex()).toStdString() << endl;

    // убираем "хвост" в начале массива
    if (big_buf[0] != DLE)
        for (int i = 0; i < big_buf.size() - 1; i++)
            if (big_buf[i] == DLE && big_buf[i + 1] == DLE)
            {
                big_buf.remove(0, i + 1);
                break;
            }

//    cout << QString(big_buf.toHex()).toStdString() << endl;


    // проверяем, есть ли в буфере целый пакет
    for (int i = 0; i < big_buf.size(); i++)
        if (big_buf[i] == DLE)
        {
            if (got_dle)
            {
                count++;
                got_dle = false;
                dle_pos[1] = i;
                // передаем позиции DLE
                extract_packs(big_buf, dle_pos);
            }
            else
            {
                got_dle = true;
                dle_pos[0] = i;
            }
        }

    // обрезаем обработанные пакеты
    big_buf.remove(0, dle_pos[1] + 1);
//    cout << QString(big_buf.toHex()).toStdString() << endl;

//    cout << "count = " << count << endl;
    buf.clear();
}

void cp9b_lib::send_to_COM(char *data, int data_size)
{
    char* data_to_send = new char[100];
    data_to_send[0] = DLE;
    int pos = 1;
    for (int i = 0; i < data_size; i++)
    {
        if (data[i] == ESC || data[i] == DLE)
        {
            data_to_send[pos] = ESC;
            pos++;
            // инвертирование 5 бита
            if (data[i] & (1 << 5))  // если 5 бит ненулевой
                data_to_send[pos] = data[i] & (~(1 << 5));
            else
                data_to_send[pos] = data[i] | (1 << 5);
        }
        else
            data_to_send[pos] = data[i];
        pos++;
    }
    data_to_send[pos] = DLE;

    // check**************************************************************************
//    QString str;
//    for(int a = 0; a < pos + 1; a++)
//    {
//        str += QString("%1 ").arg((unsigned char)data_to_send[a], 2, 16, QChar('0'));
//    }
//    cout << str.toStdString() << "   " << endl;
    // *******************************************************************************


    serial.write((const char*)data_to_send, pos + 1);

    if (timer_test_mode)
    {
        packs_sent++;
        if (packs_sent == packsnum)
            tim.stop();
    }
}

