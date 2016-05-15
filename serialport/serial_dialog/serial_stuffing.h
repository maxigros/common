#include "vars.h"
#include <QObject>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QVector>
#include <QTimer>
#include <ctime>



class serial_stuffing : public QObject
{
    Q_OBJECT

public:
    explicit serial_stuffing(QObject *parent = 0);
    ~serial_stuffing();

private:


    void extract_packs(QByteArray bb, int* dle_pos);
    char* make_random_pack(int size);


public:
    QSerialPort serial;
    QByteArray big_buf;
    void connect_to_COM(QString serialport_name);
    void send_to_COM(char *data, int data_size);


    QTimer tim;
    bool timer_test_mode = false;
    int packsnum = 100;
    int pack_size = 10;
    char test_buf[5] = {0x01, 0x7e, 0x02, 0x7d, 0x03};

    int packs_sent = 0;
    int packs_received = 0;

private slots:
    void read_COM();
    void tim_timeout();

signals:
    void send_to_gui(int key, QString msg, char *data, int data_size, int* nums, int nums_size);
};

