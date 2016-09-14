#include "cp9b_client.h"

cp9b_client::cp9b_client(QObject *parent) : QObject(parent)
{
    sock_is_open = false;
    connect(&sock, SIGNAL(readyRead()), SLOT(onSocketDataReady()));
}

void cp9b_client::open_socket()
{
    if (sock_is_open)
        sock.close();

    sock.setPortName(com_port);
    sock.setBaudRate(QSerialPort::Baud115200);
    sock.setDataBits(QSerialPort::Data8);
    sock.setParity(QSerialPort::NoParity);
    sock.setStopBits(QSerialPort::OneStop);
    sock.setFlowControl(QSerialPort::NoFlowControl);

    if (sock.open(QSerialPort::ReadWrite))
    {
        sock_is_open = true;

        emit response(0, QString::fromUtf8("Port %1 is open")
                        .arg(com_port),
                      NULL,
                      0);
    }
    else
        emit response(0, QString::fromUtf8("Error at port %1 : %2")
                        .arg(com_port)
                        .arg(sock.errorString()),
                      NULL,
                      0);
}

void cp9b_client::close_socket()
{
    sock.close();
    sock_is_open = false;
    emit response(0, QString("COM Port closed"), NULL, 0);
}

void cp9b_client::set_COM_port(QString str)
{
    com_port = str;
}

void cp9b_client::cmd_handler(cmd_data *data)
{
    if (!sock_is_open)
    {
        return;
    }
    // For similar small cmds (3 bytes)
    if (    (data->cmd == CMD_BINR_START)        ||
            (data->cmd == CMD_BINR_STOP)         ||
            (data->cmd == CMD_STATUS)            ||
            (data->cmd == CMD_STOP_SESSION)      ||
            (data->cmd == CMD_FLASH_ERASE)       ||
            (data->cmd == CMD_FLASH_MAKE_FAT)    ||
            (data->cmd == CMD_FLASH_FREE_SPACE)
            )
    {
        small_3_bytes_cmds(data->dev_addr, data->cmd);

    } else
        // For similar cmds (5 bytes), which require session name
        if ((data->cmd == CMD_START_SESSION)       ||
            (data->cmd == CMD_FLASH_SESSION_SIZE)
            )
        {
            unsigned char* session_name = new unsigned char[2];
            data->str = data->str.simplified();
            if (data->str.length() == 0)
                return;
            QStringList str_split = data->str.split(" ");
            for (int i = 0; i < 2; i++)
            {
                bool ok;
                session_name[i] = str_split[i].toInt(&ok, 16);
            }

            medium_5_bytes_cmds(data->dev_addr, data->cmd, session_name);
            delete [] session_name;
            session_name = NULL;

        } else
            // For the rest
        {
            switch (data->cmd) {
            case CMD_FLASH_READ_BLOCK:{
                flash_read_block(data->dev_addr, data->num);
                break;
                }
            case CMD_FLASH_READ_SESSION_BLOCK:{
                unsigned char* session_name = new unsigned char[2];
                data->str = data->str.simplified();
                if (data->str.length() == 0)
                    return;
                QStringList str_split = data->str.split(" ");
                for (int i = 0; i < 2; i++)
                {
                    bool ok;
                    session_name[i] = str_split[i].toInt(&ok, 16);
                }

                flash_read_session_block(data->dev_addr, session_name, data->num);
                delete [] session_name;
                session_name = NULL;
                break;
                }
            case SERVICE_CMD_MANUAL_DATA:{
                data->str = data->str.simplified();
                if (data->str.length() == 0)
                    return;
                QStringList str_split = data->str.split(" ");
                int size = str_split.size() + 2;
                char* temp = new char[size];
                temp[0] = data->dev_addr;
                temp[size - 1] = temp[0];
                for (int i = 1; i < size - 1; i++)
                {
                    bool ok;
                    temp[i] = str_split[i - 1].toInt(&ok, 16);
                    temp[size - 1] ^= temp[i];
                }
                cp9b_send_message(temp, size);
                delete [] temp;
                temp = NULL;
                }
            }

    }
}

void cp9b_client::cp9b_send_message(char *data, int size)
{
    char * data_to_send = new char[size * 2 + 2];
    data_to_send[0] = DLE;
    int pos = 1;
    for (int i = 0; i < size; i++)
    {
        if (data[i] == DLE || data[i] == ESC)
        {
            data_to_send[pos] = ESC;
            pos++;
            // invert 5th bit
            if (data[i] & (1 << 5)) // 5th bit is 1
                data_to_send[pos] = data[i] & (~(1 << 5));
            else    // 5th bit is 0
                data_to_send[pos] = data[i] | (1 << 5);
        }
        else
            data_to_send[pos] = data[i];

        pos++;
    }
    data_to_send[pos] = make_crc(data_to_send, pos);
    data_to_send[pos + 1] = DLE;

    sock.write((const char*) data_to_send);

    QString str = QString(">>>: ");
    for(int i = 0; i < size; i++)
       str += QString("%1 ").arg((unsigned char)data[i], 2, 16, QChar('0'));
    emit response(0, str, NULL, 0);

    delete [] data_to_send;
    data_to_send = NULL;
}

void cp9b_client::onSocketDataReady()
{
    QByteArray buf = sock.readAll();
    read_buf.append(buf);
//    cout << QString(big_buf.toHex()).toStdString() << endl;

    bool got_dle = false;
    int* dle_pos = new int[2];
    int count = 0;


    // убираем "хвост" в начале массива
    if (read_buf[0] != DLE)
        for (int i = 0; i < read_buf.size() - 1; i++)
            if (read_buf[i] == DLE && read_buf[i + 1] == DLE)
            {
                read_buf.remove(0, i + 1);
                break;
            }

    //    cout << QString(read_buf.toHex()).toStdString() << endl;


    // проверяем, есть ли в буфере целый пакет
    for (int i = 0; i < read_buf.size(); i++)
        if (read_buf[i] == DLE)
        {
            if (got_dle)
            {
                count++;
                got_dle = false;
                dle_pos[1] = i;
                // передаем позиции DLE
                unpack(read_buf, dle_pos);
            }
            else
            {
                    got_dle = true;
                    dle_pos[0] = i;
            }
        }

    buf.clear();
}

void cp9b_client::unpack(QByteArray bb, int* dle_pos)
{
    // обрезаем справа
    QByteArray b = bb.remove(dle_pos[1], bb.size() - dle_pos[1]);
    // обрезаем слева
    b = b.remove(0, dle_pos[0] + 1);

    for (int i = 0; i < b.size(); i++)
        if (b[i] == ESC)
        {
            // инвертирование 5 бита
            if (b[i + 1] & (1 << 5))  // если 5 бит ненулевой
                b[i + 1] = b[i + 1] & (~(1 << 5));
            else
                b[i + 1] = b[i + 1] | (1 << 5);

            b.remove(i, 1);
        }

    char* buf = new char[100];
    memcpy(buf, b.data(), b.count() * sizeof(char));
    int s = b.count();

    handle_received_package(buf, s);

    delete [] buf;
    buf = NULL;
}

void cp9b_client::onSocketError()
{
    emit response(0, sock.errorString(), NULL, 0);
    sock.close();
}

void cp9b_client::small_3_bytes_cmds(char addr, unsigned char cmd)
{
    int size = 3;
    char* temp = new char[size];
    temp[0] = addr;
    temp[1] = cmd;
    temp[2] = temp[0];
    for (int i = 1; i < size - 1; i++)
        temp[size - 1] ^= temp[i];
    cp9b_send_message(temp, size);
    delete [] temp;
    temp = NULL;
}

void cp9b_client::medium_5_bytes_cmds(char addr, unsigned char cmd, unsigned char *session_name)
{
    int size = 5;
    char* temp = new char[size];
    temp[0] = addr;
    temp[1] = cmd;
    temp[2] = session_name[0];
    temp[3] = session_name[1];
    temp[4] = temp[0];
    for (int i = 1; i < size - 1; i++)
        temp[size - 1] ^= temp[i];
    cp9b_send_message(temp, size);
    delete [] temp;
    temp = NULL;
}

void cp9b_client::flash_read_block(char addr, int block_num)
{
    int size = 7;
    char* temp = new char[size];
    temp[0] = addr;
    temp[1] = CMD_FLASH_READ_BLOCK;
    temp[2] = block_num & 0xFF;
    temp[3] = (block_num >> 8) & 0xFF;
    temp[4] = (block_num >> 16) & 0xFF;
    temp[5] = (block_num >> 24) & 0xFF;
    temp[6] = temp[0];
    for (int i = 1; i < size - 1; i++)
        temp[size - 1] ^= temp[i];
    cp9b_send_message(temp, size);
    delete [] temp;
    temp = NULL;
}

void cp9b_client::flash_read_session_block(char addr, unsigned char *session_name, int session_block_num)
{
    int size = 9;
    char* temp = new char[size];
    temp[0] = addr;
    temp[1] = CMD_FLASH_READ_SESSION_BLOCK;
    temp[2] = session_name[0];
    temp[3] = session_name[1];
    temp[4] = session_block_num & 0xFF;
    temp[5] = (session_block_num >> 8) & 0xFF;
    temp[6] = (session_block_num >> 16) & 0xFF;
    temp[7] = (session_block_num >> 24) & 0xFF;
    temp[8] = temp[0];
    for (int i = 1; i < size - 1; i++)
        temp[size - 1] ^= temp[i];
    cp9b_send_message(temp, size);
    delete [] temp;
    temp = NULL;
}

char cp9b_client::make_crc(char *temp, int s)
{
    char crc = temp[0];
    for (int i = 1; i < s; i++)
        crc = (crc ^ temp[i]) & 0xFF;
    return crc;
}

void cp9b_client::handle_received_package(char* buf, int size)
{
    QString msg = QString("<<<: <%1>: ").arg(com_port);
    for(int i = 0; i < size; i++)
        msg += QString("%1 ").arg((unsigned char)buf[i], 2, 16, QChar('0'));

    int key = 0;

    switch ((unsigned char)buf[0]) {
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

    emit response(key, msg, buf, size);
}
