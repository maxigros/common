#include "llmdb_client_ext.h"

llmdb_client_ext::llmdb_client_ext(QObject *parent) : llmdb_client(parent)
{
    mode_none = true;
    mode_flash_download = false;
    mode_sessions_rec = false;
    mode_session_download = false;
    connect(&timer_repeat, SIGNAL(timeout()), this, SLOT(timer_repeat_timeout()));
    connect(&timer_session_rec_duration, SIGNAL(timeout()), this, SLOT(timer_session_rec_duration_timeout()));
}

void llmdb_client_ext::initiate_mode(int new_mode, unsigned char addr, int *data, QString str)
{
    next_message.dev_addr = addr;
    switch (new_mode) {
    case MODE_SESSIONS_REC:
        start_sessions_rec(data);
        break;
    case MODE_SESSION_DOWNLOAD:
        mode_none = false;
        mode_flash_download = false;
        mode_sessions_rec = false;
        mode_session_download = true;
        break;
    case MODE_FLASH_DOWNLOAD:
        mode_none = false;
        mode_flash_download = true;
        mode_sessions_rec = false;
        mode_session_download = false;
        break;
    case MODE_NONE:
        mode_none = true;
        mode_flash_download = false;
        mode_sessions_rec = false;
        mode_session_download = false;
        timer_session_rec_duration.stop();
        timer_repeat.stop();
        break;
    default:
        break;
    }
}

void llmdb_client_ext::start_sessions_rec(int *data)
{
    mode_none = false;
    mode_flash_download = false;
    mode_sessions_rec = true;
    mode_session_download = false;

    sessions_rec_counter = 1;
    sessions_rec_quantity = data[0];

    timer_repeat.start(LLMDB_TIMEOUT);
    small_3_bytes_cmds(next_message.dev_addr, CMD_BINR_START);
}

void llmdb_client_ext::onSocketDataReady()
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

        if (!mode_none)
        {
            timer_repeat.stop();
            if (mode_sessions_rec)
            {
                switch (buf[0]) {
                case ((((CMD_BINR_START >> 3) + CMD_BINR_START) & 0x0F) | (0x00 << 4)):{
                    unsigned char temp1 = (sessions_rec_counter >> 8) & 0xFF;
                    unsigned char temp2 = sessions_rec_counter & 0xFF;
                    next_message.str = QString("%1 %2")
                                                    .arg(temp1)
                                                    .arg(temp2);
                    next_message.cmd = CMD_START_SESSION;
                    timer_repeat.start(LLMDB_TIMEOUT);
                    cmd_handler(next_message);
                    break;
                    }
                case ((((CMD_START_SESSION >> 3) + CMD_START_SESSION) & 0x0F) | (0x00 << 4)):{
                    next_message.cmd = CMD_STOP_SESSION;
                    timer_session_rec_duration.start(SESSION_DURATION);
                    session_rec_in_progress = true;
                    break;
                    }
                case ((((CMD_STOP_SESSION >> 3) + CMD_STOP_SESSION) & 0x0F) | (0x00 << 4)):{
                    sessions_rec_counter++;
                    if (sessions_rec_counter > sessions_rec_quantity)
                    {
                        key = KEY_AUTO_MODE_OFF;
                        session_rec_in_progress = false;
                        emit response(key, NULL, NULL, 0);
                        return;
                    }
                    unsigned char temp1 = (sessions_rec_counter >> 8) & 0xFF;
                    unsigned char temp2 = sessions_rec_counter & 0xFF;
                    next_message.str = QString("%1 %2")
                                                    .arg(temp1)
                                                    .arg(temp2);
                    next_message.cmd = CMD_START_SESSION;
                    cmd_handler(next_message);
                    timer_repeat.start(LLMDB_TIMEOUT);
                    break;
                    }
                default:
                    break;
                }
            }
        }

        delete [] buf;
        buf = NULL;
    }
}

void llmdb_client_ext::timer_repeat_timeout()
{
    cmd_handler(next_message);
    timer_repeat.start(LLMDB_TIMEOUT);
}

void llmdb_client_ext::timer_session_rec_duration_timeout()
{
    cmd_handler(next_message);
    timer_session_rec_duration.stop();
}
