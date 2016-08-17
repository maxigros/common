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
        emit response(0, QString::fromUtf8("Socket opened on port: ")
                 + QString::number(sock.localPort()), NULL, 0);

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
    emit response(0, QString("Socket closed"), NULL, 0);
}

void llmdb_client::cmd_handler(cmd_data data)
{
    // For similar small cmds (3 bytes)
    if (    (data.cmd == CMD_BINR_START)        ||
            (data.cmd == CMD_BINR_STOP)         ||
            (data.cmd == CMD_STATUS)            ||
            (data.cmd == CMD_STOP_SESSION)      ||
            (data.cmd == CMD_FLASH_ERASE)       ||
            (data.cmd == CMD_FLASH_MAKE_FAT)    ||
            (data.cmd == CMD_FLASH_FREE_SPACE)
            )
    {
        int size = 3;
        char* temp = new char[size];
        temp[0] = data.dev_addr;
        temp[1] = data.cmd;
        temp[2] = temp[0];
        for (int i = 1; i < size - 1; i++)
            temp[size - 1] ^= temp[i];
        llmdb_send_message(temp, size);
        delete [] temp;
        temp = NULL;

    } else
        // For similar cmds (5 bytes), which require session name
        if ((data.cmd == CMD_START_SESSION)       ||
            (data.cmd == CMD_FLASH_SESSION_SIZE)
            )
        {
            unsigned char* session_name = new unsigned char[2];
            data.str = data.str.simplified();
            if (data.str.length() == 0)
                return;
            QStringList str_split = data.str.split(" ");
            for (int i = 0; i < 2; i++)
            {
                bool ok;
                session_name[i] = str_split[i].toInt(&ok, 16);
            }

            int size = 5;
            char* temp = new char[size];
            temp[0] = data.dev_addr;
            temp[1] = data.cmd;
            temp[2] = session_name[0];
            temp[3] = session_name[1];
            temp[4] = temp[0];
            for (int i = 1; i < size - 1; i++)
                temp[size - 1] ^= temp[i];
            llmdb_send_message(temp, size);
            delete [] temp;
            delete [] session_name;
            temp = NULL;
            session_name = NULL;

        } else
            // For the rest
        {
            switch (data.cmd) {
            case CMD_FLASH_READ_BLOCK:{
                int size = 7;
                char* temp = new char[size];
                temp[0] = data.dev_addr;
                temp[1] = data.cmd;
                temp[2] = data.num & 0xFF;
                temp[3] = (data.num >> 8) & 0xFF;
                temp[4] = (data.num >> 16) & 0xFF;
                temp[5] = (data.num >> 24) & 0xFF;
                temp[6] = temp[0];
                for (int i = 1; i < size - 1; i++)
                    temp[size - 1] ^= temp[i];
                llmdb_send_message(temp, size);
                delete [] temp;
                temp = NULL;
                break;
                }
            }

    }
}

void llmdb_client::llmdb_send_message(char *msg, int size)
{
    sock.writeDatagram((char*)msg, size, QHostAddress(host_addr), 1001);

    QString str;
    for(int i = 0; i < size; i++)
       str += QString("%1 ").arg((unsigned char)msg[i], 2, 16, QChar('0'));
    emit response(0, str, NULL, 0);
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
        for(int i = 0; i < s; i++)
            msg += QString("%1 ").arg((unsigned char)buf[i], 2, 16, QChar('0'));

        int key = 0;

        switch (buf[0]) {
        case ((((CMD_STATUS >> 3) + CMD_STATUS) & 0x0F) | (0x03 << 4)):
            key = KEY_STATUS;
            break;
        case ((((CMD_FLASH_SESSION_SIZE >> 3) + CMD_FLASH_SESSION_SIZE) & 0x0F)
                    | (0x03 << 4)):
            key = KEY_SESSION_SIZE;
            break;
        case ((((CMD_FLASH_FREE_SPACE >> 3) + CMD_FLASH_FREE_SPACE) & 0x0F)
                    | (0x03 << 4)):
            key = KEY_FLASH_FREE_SPACE;
            break;
        default:
            key = 0;
            break;
        }

        emit response(key, msg, buf, s);
        delete [] buf;
        buf = NULL;
    }
}

void llmdb_client::onSocketError()
{
    emit response(0, sock.errorString(), NULL, 0);
    sock.close();
}
