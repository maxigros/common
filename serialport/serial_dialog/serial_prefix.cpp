#include "serial_prefix.h"
#include <iostream>
using namespace std;


serial_prefix::serial_prefix(QObject *parent) :
    QObject(parent)
{
    connect(&serial, SIGNAL(readyRead()), this, SLOT(read_COM()));
    connect(&tim, SIGNAL(timeout()), this, SLOT(tim_timeout()));
}

serial_prefix::~serial_prefix()
{
    if (serial.isOpen())
        serial.close();
}

void serial_prefix::extract_packs(QByteArray bb, int prefix_pos)
{
    bb.remove(0, prefix_size);
    char *temp = new char[2];
    memcpy(temp, bb.data(), 2 * sizeof(char));

    unsigned short int rec_pack_size = ((int)temp[0] << 8) | (int)temp[1];

    bb.remove(0, 2);

    // если есть целый пакет
    if (bb.size() >= (rec_pack_size))
    {
        char *temp2 = new char[rec_pack_size];
        memcpy(temp2, bb.data(), rec_pack_size * sizeof(char));

        if (timer_test_mode)
        {
            // проверка правильности пакета
            char crc = temp2[0];
            for (int i = 1; i < rec_pack_size - 1; i++)
                crc ^= temp2[i];
            if (crc == temp2[rec_pack_size - 1])
                packs_received++;

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
                                 QString("Timetest off"),
                                 0,
                                 0,
                                 n,
                                 2);
            }
        }
        int s = rec_pack_size - 1;
        emit send_to_gui(KEY_MSG_FROM_DEV, 0, temp2, s, 0, 0);
        delete temp2;
    }
    delete temp;
    big_buf.remove(0, prefix_size + 2 + rec_pack_size);
}

char *serial_prefix::make_random_pack(int size)
{

    char *data = new char[size];
    srand(time(0));
    data[size - 1] = 0;
    for (int i = 0; i < size - 1;i++)
    {
        data[i] = rand() % 255;
        // CRC
        data[size - 1] = (data[size - 1] + data[i]) & 0xFF;
    }

    return data;

}

void serial_prefix::tim_timeout()
{

    send_to_COM(make_random_pack(pack_size), pack_size);

}

void serial_prefix::connect_to_COM(QString serialport_name)
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
                         QString("Port %1 is opened (Prefix mode)")
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
                         QString("Error at port %1 : %2 (Prefix mode)")
                                    .arg(serialport_name)
                                    .arg(str),
                         0,
                         0,
                         0,
                         0);
    }

}

void serial_prefix::read_COM()
{
    QByteArray buf = serial.readAll();
    big_buf.append(buf);
    cout << QString(big_buf.toHex()).toStdString() << endl;

    bool got_prefix = false;
    int prefix_counter = 0;
    unsigned char previous_byte = 0;
    int prefix_end = 0;

    // обрубаем хвост
    if (big_buf.size() >= prefix_size)
        for (int i = 0; i < big_buf.size(); i++)
        {
            if (big_buf[i] == PREFIX_BYTE)
            {
                if (previous_byte == PREFIX_BYTE)
                    prefix_counter++;
                if (prefix_counter == prefix_size - 1)
                {
                    prefix_end = i;
                    big_buf.remove(0, prefix_end - prefix_size + 1);
                    break;
                }
            }
            else
                prefix_counter = 0;
            previous_byte = big_buf[i];
        }
    previous_byte = 0;

    if (big_buf.size() >= (prefix_size + 2))
    {
        for (int i = 0; i < big_buf.size(); i++)
        {
            if (big_buf[i] == PREFIX_BYTE)
            {
                if (previous_byte == PREFIX_BYTE)
                    prefix_counter++;

                if (prefix_counter == prefix_size - 1)
                {
                    extract_packs(big_buf, i);
                    prefix_counter = 0;
                }
            }
            else
                prefix_counter = 0;
            previous_byte = big_buf[i];
        }
    }
}

void serial_prefix::send_to_COM(char *data, int data_size)
{
    // set prefix
    for (int i = 0; i < prefix_size; i++)
        prefix[i] = PREFIX_BYTE;

    int send_size = prefix_size + 2 + data_size + 1;
    char* data_to_send = new char[prefix_size + 2 + pack_size + 1];
    for (int i = 0; i < prefix_size; i++)
        data_to_send[i] = prefix[i];

    // package size
    data_to_send[prefix_size + 1] = (data_size + 1) & 0xFF;
    data_to_send[prefix_size] = ((data_size + 1) >> 8) & 0xFF;

    int pos = prefix_size + 2;

    // CRC
    char crc = data[0];
    for (int i = 1; i < data_size; i++)
        crc ^= data[i];
    data_to_send[send_size - 1] = crc;

    for (int i = 0; i < data_size; i++)
    {
        data_to_send[pos + i] = data[i];
    }

    // check**************************************************************************
    QString str;
    for(int a = 0; a < send_size; a++)
    {
        str += QString("%1 ").arg((unsigned char)data_to_send[a], 2, 16, QChar('0'));
    }
    emit send_to_gui(KEY_MSG_FROM_CLI, str, 0, 0, 0, 0);
//    cout << str.toStdString() << "   " << endl;
    // *******************************************************************************


    serial.write((const char*)data_to_send, send_size);
    delete data_to_send;

    if (timer_test_mode)
    {
        packs_sent++;

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


        if (packs_sent == packsnum)
            tim.stop();
    }

}
