#include "llmdb_client_ext.h"

llmdb_client_ext::llmdb_client_ext(QObject *parent) : llmdb_client(parent)
{
    mode_none = true;
    mode_flash_download = false;
    mode_sessions_rec = false;
    mode_session_download = false;

    connect(&timer_repeat, SIGNAL(timeout()),
            this, SLOT(timer_repeat_timeout()));
    connect(&timer_session_rec_duration, SIGNAL(timeout()),
            this, SLOT(timer_session_rec_duration_timeout()));
    connect(&timer_flash_status, SIGNAL(timeout()),
            this, SLOT(timer_flash_status_timeout()));
}

void llmdb_client_ext::initiate_mode(mode_task *task)
{
    next_message.dev_addr = task->dev_addr;
    switch (task->mode) {
    case MODE_SESSIONS_REC:
        start_sessions_rec(task->data);
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
        timers_stop();
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
    // Set duration mode
    if (data[1]) // random
    {

    }
    else // manual
        sessions_rec_duration = data[2];

    // Set status request mode
    if (data[3])
        timer_flash_status.start(data[4] * 1000);

    sessions_rec_contents_check_mode = data[5];

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
                    if (!sessions_rec_contents_check_mode)
                        // no contents check
                    {
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
                    }
                    else
                        // contents check
                    {
                        next_message.cmd = CMD_FLASH_READ_BLOCK;
                        next_message.num = 1;
                        cmd_handler(next_message);
                        timer_repeat.start(LLMDB_TIMEOUT);
                    }
                    break;
                }
                case ((((CMD_FLASH_READ_BLOCK >> 3) + CMD_FLASH_READ_BLOCK) & 0x0F) | (0x04 << 4)):
                // may occur only if contents check is on
                {
                    // contents wrong
                    if (!check_contents(buf))
                    {
                        mode_task* temp = new mode_task();
                        temp->mode = MODE_NONE;
                        initiate_mode(temp);
                        delete temp;
                        temp = NULL;

                        key = KEY_AUTO_MODE_OFF;
                        session_rec_in_progress = false;
                        emit response(key, NULL, NULL, 0);

                        key = KEY_FLASH_CONTENTS_ERROR;
                        emit response(key, NULL, NULL, 0);
                        return;
                    }

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

void llmdb_client_ext::timers_stop()
{
    timer_repeat.stop();
    timer_session_rec_duration.stop();
    timer_flash_status.stop();
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

void llmdb_client_ext::timer_flash_status_timeout()
{
    cmd_data* temp_message = new cmd_data();
    temp_message->dev_addr = next_message.dev_addr;
    temp_message->cmd = CMD_STATUS;
    delete temp_message;
    temp_message = NULL;
}

bool llmdb_client_ext::check_contents(char *data)
{
    bool ok = false;
    // if ...
    return ok;
}
