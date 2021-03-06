#ifndef CP9B_CLIENT_H
#define CP9B_CLIENT_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "defines.h"


class cp9b_client : public QObject
{
    Q_OBJECT
public:
    explicit cp9b_client(QObject *parent = 0);

    void open_socket();
    void close_socket();
    void set_COM_port(QString str);
    void cmd_handler(cmd_data *data);

protected:
    QSerialPort      sock;
    QString com_port;
    QByteArray read_buf;
    bool sock_is_open;
    void cp9b_send_message(char* data, int size);


    void small_3_bytes_cmds(char addr, unsigned char cmd);
    void medium_5_bytes_cmds(char addr, unsigned char cmd, unsigned char* session_name);
    void flash_read_block(char addr, int block_num);
    void flash_read_session_block(char addr, unsigned char* session_name, int session_block_num);
    char make_crc(char* temp, int s);

signals:
    void response(int key, QString msg, char *data, int size);

private slots:
    virtual void handle_received_package(char* buf, int size) = 0;
    void onSocketDataReady();
    void unpack(QByteArray bb, int* dle_pos);
    void onSocketError();
};

#endif // CP9B_CLIENT_H
