#include "serial_stuffing.h"
#include <iostream>
using namespace std;

serial_stuffing::serial_stuffing(QObject *parent) :
    QObject(parent)
{
    connect(&serial, SIGNAL(readyRead()), this, SLOT(read_COM()));
    connect(&tim, SIGNAL(timeout()), this, SLOT(tim_timeout()));
}

serial_stuffing::~serial_stuffing()
{
    if (serial.isOpen())
        serial.close();
}

void serial_stuffing::extract_packs(QByteArray bb, int* dle_pos)
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

    char* buf = new char[b.size()];
//    char* buf = new char[50];
    memcpy(buf, b.data(), b.count());
    int s = b.count();

    // проверка правильности пришедшего пакета
    if (timer_test_mode)
    {
//        packs_received++;
        char crc = 0;
        for (int i = 0; i < s - 1; i++)
            crc ^= buf[i];
        if (crc == buf[s - 1])
            packs_received++;

//        for (int i = 0; i < s; i++)
//            if (buf[i] != test_buf[i])
//            {
//                packs_received--;
//                break;
//            }

        // в статусбар
        int n[2];
        n[0] = packs_sent;
        n[1] = packs_received;
        emit send_to_gui(KEY_MSG_PACKSNUM,
                         0,
                         0,
                         0,
                         n,
                         2);


        if (!tim.isActive())
        {
            timer_test_mode = false;
            int n[2];
            n[0] = packs_sent;
            n[1] = packs_received;
            emit send_to_gui(KEY_MSG_PACKSNUM_END,
                             0,
                             0,
                             0,
                             n,
                             2);
        }
    }

    emit send_to_gui(KEY_MSG_FROM_DEV, 0, buf, s, 0, 0);
    b.clear();
    delete buf;

    // обрезаем обработанные пакеты
    big_buf.remove(0, dle_pos[1] + 1);
}

char *serial_stuffing::make_random_pack(int size)
{
    char *data = new char[size];
    srand(time(0));
//    data[size - 1] = 0;
    for (int i = 0; i < size;i++)
    {
        data[i] = rand() % 255;
        // CRC
//        data[size - 1] ^= data[i];
    }

    return data;
}

void serial_stuffing::tim_timeout()
{

    send_to_COM(make_random_pack(pack_size), pack_size);
}

void serial_stuffing::connect_to_COM(QString serialport_name)
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
                         QString("Port %1 is opened (Stuffing mode)")
                                    .arg(serialport_name),
                         0,
                         0,
                         0,
                         0);
    }
    else
    {
        QString str = serial.errorString();
        emit send_to_gui(KEY_MSG_FROM_CLI,
                         QString("Error at port %1 : %2 (Stuffing mode)")
                                    .arg(serialport_name)
                                    .arg(str),
                         0,
                         0,
                         0,
                         0);
    }
}

void serial_stuffing::read_COM()
{
    QByteArray buf = serial.readAll();
    big_buf.append(buf);
//    cout << QString(big_buf.toHex()).toStdString() << endl;

    bool got_dle = false;
    int* dle_pos = new int[2];
    int count = 0;


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



    buf.clear();
}

void serial_stuffing::send_to_COM(char *data, int data_size)
{
    int t_size = data_size + 1;
    char *t = new char[t_size];
    for (int i = 0; i < data_size; i++)
        t[i] = data[i];

    // CRC
    char crc = t[0];
    for (int i = 1; i < t_size - 1; i++)
        crc ^= t[i];
    t[t_size - 1] = crc;


    char* data_to_send = new char[pack_size + pack_size / 3 + 1];
    data_to_send[0] = DLE;
    int pos = 1;
    for (int i = 0; i < t_size; i++)
    {
        if (t[i] == ESC || t[i] == DLE)
        {
            data_to_send[pos] = ESC;
            pos++;
            // инвертирование 5 бита
            if (t[i] & (1 << 5))  // если 5 бит ненулевой
                data_to_send[pos] = t[i] & (~(1 << 5));
            else
                data_to_send[pos] = t[i] | (1 << 5);
        }
        else
            data_to_send[pos] = t[i];
        pos++;
    }
    data_to_send[pos] = DLE;
    delete t;

    // check**************************************************************************
    QString str;
    for(int a = 0; a < pos + 1; a++)
    {
        str += QString("%1 ").arg((unsigned char)data_to_send[a], 2, 16, QChar('0'));
    }
    emit send_to_gui(KEY_MSG_FROM_CLI, str, 0, 0, 0, 0);
//    cout << str.toStdString() << "   " << endl;
    // *******************************************************************************
    delete data_to_send;

    serial.write((const char*)data_to_send, pos + 1);

    if (timer_test_mode)
    {
        packs_sent++;
        if (packs_sent == packsnum)
        {
            tim.stop();
        }
    }
}

