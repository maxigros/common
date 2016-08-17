#include "llmdb_client.h"

llmdb_client::llmdb_client(QObject *parent) : QObject(parent)
{
    connect(&sock, SIGNAL(readyRead()), SLOT(onSocketDataReady()));
}

void llmdb_client::open_socket()
{
    sock.close();

    if(sock.bind())
    {
        response(QString::fromUtf8("Socket opened on port: ")
                 + QString::number(sock.localPort()));

        // Отключить хардваре эхо (4 в параметре №10, RS485MUX0)
        unsigned char rq1[] = {0x9a, 0x1f, 0x50, 0x0a, 0x04, 0xed, 0x28 };
        // Задать скорость порта 1 (25 в параметре №80, L0_BR)
        unsigned char rq2[] = {0x9a, 0x20, 0x50, 0x50, 0x19, 0xac, 0x78 };

        sock.writeDatagram((char*)rq1, 7, QHostAddress(host_addr), 1000);
        sock.writeDatagram((char*)rq2, 7, QHostAddress(host_addr), 1000);
    }
}

void llmdb_client::close_socket()
{
    sock.close();
    response(QString("Socket closed"));
}

void llmdb_client::onSocketDataReady()
{
    QHostAddress addr;
    quint16 port;
    while(sock.hasPendingDatagrams())
    {
        int s = sock.pendingDatagramSize();
        char* buf = new char[s];
        sock.readDatagram(buf, s, &addr, &port);
        QString msg = QString("<%1>: ").arg(port);
        for(int a=0; a<s; a++)
        {
            msg += QString("%1 ").arg((unsigned char)buf[a], 2, 16, QChar('0'));
        }
        response(msg);
    }
}

void llmdb_client::onSocketError()
{

}
