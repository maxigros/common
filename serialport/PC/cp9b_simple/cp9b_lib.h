#ifndef CP9B_LIB_H
#define CP9B_LIB_H

#include <QObject>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>

#define KEY_MSG_FROM_DEV    1
#define KEY_MSG_FROM_CLI    2

class cp9b_lib : public QObject
{
    Q_OBJECT

public:
    explicit cp9b_lib(QObject *parent = 0);
    ~cp9b_lib();

private:
    QSerialPort serial;

public:
    void connect_to_COM(QString serialport_name);
    void send_to_COM(char *data, int data_size);

private slots:
    void read_COM();

signals:
    void send_to_gui(int key, QString msg, char *data, int data_size);
};

#endif // CP9B_LIB_H
