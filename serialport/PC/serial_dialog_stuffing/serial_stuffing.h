#ifndef CP9B_LIB_H
#define CP9B_LIB_H

#include <QObject>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QVector>
#include <QTimer>
#include <ctime>

#define KEY_MSG_FROM_DEV    1
#define KEY_MSG_FROM_CLI    2
#define KEY_MSG_PACKSNUM    3
#define DLE    0x7e
#define ESC    0x7d

class cp9b_lib : public QObject
{
    Q_OBJECT

public:
    explicit cp9b_lib(QObject *parent = 0);
    ~cp9b_lib();

private:
    QSerialPort serial;
    QByteArray big_buf;

    void extract_packs(QByteArray bb, int* dle_pos);
    char* make_random_pack(int size);


public:
    void connect_to_COM(QString serialport_name);
    void send_to_COM(char *data, int data_size);

    int packs_sent = 0;
    int packs_received = 0;
    QTimer tim;
    bool timer_test_mode = false;
    int packsnum = 100;
    int pack_size = 10;
    char test_buf[5] = {0x01, 0x7e, 0x02, 0x7d, 0x03};

private slots:
    void read_COM();
    void tim_timeout();

signals:
    void send_to_gui(int key, QString msg, char *data, int data_size);
};

#endif // CP9B_LIB_H
