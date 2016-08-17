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

    void llmdb_send_message(char* msg, int size);


signals:
    void response(int key, QString msg, char *data, int size);

private slots:

    void onSocketDataReady();
    void onSocketError();

};

#endif // LLMDB_CLIENT_H
