#ifndef LLMDB_CLIENT_H
#define LLMDB_CLIENT_H

#include <QUdpSocket>
#include <QTimer>
#include "defines.h"


class llmdb_client : public QObject
{
    Q_OBJECT
public:
    explicit llmdb_client(QObject *parent = 0);

    void open_socket();
    void close_socket();
    void cmd_handler(cmd_data data);

    QString host_addr;


private:
    QUdpSocket      sock;
    QTimer timer_repeat;
    QTimer timer_flash_ask_free_space;

    void llmdb_send_message(char* msg, int size);


signals:
    void response(int key, QString msg, char *data, int size);

private slots:

    void onSocketDataReady();
    void onSocketError();
    void small_3_bytes_cmds(char addr, unsigned char cmd);
    void medium_5_bytes_cmds(char addr, unsigned char cmd, unsigned char* session_name);
    void flash_read_block(char addr, int block_num);
    void flash_read_session_block(char addr, unsigned char* session_name, int session_block_num);

};

#endif // LLMDB_CLIENT_H
