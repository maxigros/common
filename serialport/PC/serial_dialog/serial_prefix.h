#include "vars.h"
#include <QObject>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QVector>
#include <QTimer>
#include <ctime>



class serial_prefix : public QObject
{
    Q_OBJECT

public:
    explicit serial_prefix(QObject *parent = 0);
    ~serial_prefix();

private:


    void extract_packs(QByteArray bb, int prefix_pos);
    char* make_random_pack(int size);
    char prefix[10];


public:
    QSerialPort serial;
    QByteArray big_buf;
    void connect_to_COM(QString serialport_name);
    void send_to_COM(char *data, int data_size);

    int packs_sent = 0;
    int packs_received = 0;
    QTimer tim;
    bool timer_test_mode = false;
    int packsnum = 100;
    int pack_size = 50;
    int prefix_size = 2;

private slots:
    void read_COM();
    void tim_timeout();

signals:
    void send_to_gui(int key, QString msg, char *data, int data_size, int* nums, int nums_size);
};
