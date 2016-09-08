#include "llmdb_client_ext.h"
#include <ctime>

llmdb_client_ext::llmdb_client_ext(QObject *parent) : llmdb_client(parent)
{
    mode_none = true;
    mode_flash_download = false;
    mode_sessions_rec = false;
    mode_session_download = false;
    next_message = new cmd_data;

    connect(&timer_repeat, SIGNAL(timeout()),
            this, SLOT(timer_repeat_timeout()));
    connect(&timer_session_rec_duration, SIGNAL(timeout()),
            this, SLOT(timer_session_rec_duration_timeout()));
    connect(&timer_flash_status, SIGNAL(timeout()),
            this, SLOT(timer_flash_status_timeout()));
}

void llmdb_client_ext::initiate_mode(mode_task *task)
{
    next_message->dev_addr = task->dev_addr;

    switch (task->mode) {
    case MODE_SESSIONS_REC:
        start_sessions_rec(task->data);
        break;
    case MODE_SESSION_DOWNLOAD:
        start_session_download(task->str);
        break;
    case MODE_FLASH_DOWNLOAD:
        start_flash_download(task->data);
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

void llmdb_client_ext::onSocketDataReady()
{
    QHostAddress addr;
    quint16 port;
    while(sock.hasPendingDatagrams())
    {
        int s = sock.pendingDatagramSize();
        char* buf = new char[s];
        sock.readDatagram(buf, s, &addr, &port);
        QString msg = QString("<<<: <%1>: ").arg(port);
        for(int i = 0; i < s; i++)
            msg += QString("%1 ").arg((unsigned char)buf[i], 2, 16, QChar('0'));

        int key = 0;

        switch ((unsigned char)buf[0]) {
        case ((((CMD_STATUS >> 3) +
                 CMD_STATUS) & 0x0F) |
                 (0x03 << 4)):
        {
            key = KEY_STATUS;
            break;
        }
        case ((((CMD_FLASH_SESSION_SIZE >> 3) +
                 CMD_FLASH_SESSION_SIZE) & 0x0F) |
                 (0x03 << 4)):
        {
            key = KEY_SESSION_SIZE;
            break;
        }
        case ((((CMD_FLASH_FREE_SPACE >> 3) +
                 CMD_FLASH_FREE_SPACE) & 0x0F) |
                 (0x03 << 4)):
        {
            key = KEY_FLASH_FREE_SPACE;
            break;
        }
        case ((((CMD_FLASH_SESSION_SIZE >> 3) +
                 CMD_FLASH_SESSION_SIZE) & 0x0F) |
                 (0x0e << 4)):
        {
            if (!mode_none)
            {
                mode_none = true;
                key = KEY_AUTO_MODE_OFF;
                emit response(key, NULL, NULL, 0);
            }
            key = KEY_SESSION_SIZE_ERROR;
            break;
        }
        case ((((CMD_FLASH_READ_SESSION_BLOCK >> 3) +
                 CMD_FLASH_READ_SESSION_BLOCK) & 0x0F) |
                 (0x03 << 4)):
        {
            if (mode_session_download)
                key = KEY_SESSION_DOWNLOAD_LOG;
            break;
        }
        case ((((CMD_FLASH_READ_BLOCK >> 3) +
                 CMD_FLASH_READ_BLOCK) & 0x0F) |
                 (0x03 << 4)):
        {
            if (mode_flash_download)
                key = KEY_FLASH_DOWNLOAD_LOG;
            break;
        }
        default:
        {
            key = 0;
            break;
        }
        }

        emit response(key, msg, buf, s);

        if (!mode_none)
        {
            if (mode_sessions_rec)
            {
                switch (buf[0]) {
                case ((((CMD_BINR_START >> 3) + CMD_BINR_START) & 0x0F) | (0x00 << 4)):{
                    timer_repeat.stop();
                    unsigned char temp1 = (sessions_rec_counter >> 8) & 0xFF;
                    unsigned char temp2 = sessions_rec_counter & 0xFF;
                    next_message->str = QString("%1 %2")
                                                    .arg(temp1)
                                                    .arg(temp2);
                    next_message->cmd = CMD_START_SESSION;
                    timer_repeat.start(LLMDB_TIMEOUT);
                    cmd_handler(next_message);
                    break;
                }
                case ((((CMD_START_SESSION >> 3) + CMD_START_SESSION) & 0x0F) | (0x00 << 4)):{
                    timer_repeat.stop();
                    next_message->cmd = CMD_STOP_SESSION;
                    if (sessions_rec_duration_random)
                    {
                        srand(time(NULL));
                        sessions_rec_duration = rand() % 7 + 3;     // sec
                    }
                    timer_session_rec_duration.start(sessions_rec_duration * 1000);

                    session_rec_in_progress = true;
                    break;
                }
                case ((((CMD_STOP_SESSION >> 3) + CMD_STOP_SESSION) & 0x0F) | (0x00 << 4)):{
                    timer_repeat.stop();
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
                        next_message->str = QString("%1 %2")
                                                        .arg(temp1)
                                                        .arg(temp2);
                        next_message->cmd = CMD_START_SESSION;
                        cmd_handler(next_message);
                        timer_repeat.start(LLMDB_TIMEOUT);
                    }
                    else
                        // contents check
                    {
                        sessions_rec_contents_counter = 1;
                        next_message->cmd = CMD_FLASH_READ_BLOCK;
                        next_message->num = sessions_rec_contents_counter;
                        cmd_handler(next_message);
                        timer_repeat.start(LLMDB_TIMEOUT);
                    }
                    break;
                }
                case ((((CMD_FLASH_READ_BLOCK >> 3) + CMD_FLASH_READ_BLOCK) & 0x0F) | (0x03 << 4)):
                // may occur only if contents check is on
                {
                    timer_repeat.stop();

#ifdef OLD_CONTENTS_CHECK
                    // contents wrong
                    if (!check_contents(&buf[5]))
                    {
                        mode_task* temp = new mode_task;
                        temp->mode = MODE_NONE;
                        initiate_mode(temp);
                        delete temp;
                        temp = NULL;

                        key = KEY_AUTO_MODE_OFF;
                        session_rec_in_progress = false;
                        emit response(key, NULL, NULL, 0);

                        key = KEY_FLASH_CONTENTS_ERROR;
                        emit response(key, NULL, NULL, 0);
                        delete [] buf;
                        buf = NULL;
                        return;
                    }
#endif
#ifndef OLD_CONTENTS_CHECK
                    sessions_rec_contents_counter++;

                    if (sessions_rec_contents_counter <= FLASH_BLOCKS_ON_PAGE_NUMBER)
                    {
                        next_message->num = sessions_rec_contents_counter;
                        cmd_handler(next_message);
                        timer_repeat.start(LLMDB_TIMEOUT);
                        delete [] buf;
                        buf = NULL;
                        return;
                    }
#endif

                    sessions_rec_counter++;
                    if (sessions_rec_counter > sessions_rec_quantity)
                    {
                        key = KEY_AUTO_MODE_OFF;
                        session_rec_in_progress = false;
                        emit response(key, NULL, NULL, 0);
                        delete [] buf;
                        buf = NULL;
                        return;
                    }

                    // no free space request
                    if (!sessions_rec_free_space_requset_mode)
                        {
                        unsigned char temp1 = (sessions_rec_counter >> 8) & 0xFF;
                        unsigned char temp2 = sessions_rec_counter & 0xFF;
                        next_message->str = QString("%1 %2")
                                                        .arg(temp1)
                                                        .arg(temp2);
                        next_message->cmd = CMD_START_SESSION;
                        cmd_handler(next_message);
                        timer_repeat.start(LLMDB_TIMEOUT);
                    }
                    else
                    {
                        next_message->cmd = CMD_FLASH_FREE_SPACE;
                        cmd_handler(next_message);
                        timer_repeat.start(LLMDB_TIMEOUT);
                    }
                }
                case ((((CMD_FLASH_FREE_SPACE >> 3) + CMD_FLASH_FREE_SPACE) & 0x0F) | (0x03 << 4)):
                    // may occur only if free space request is on
                {
                    timer_repeat.stop();
                    unsigned char temp1 = (sessions_rec_counter >> 8) & 0xFF;
                    unsigned char temp2 = sessions_rec_counter & 0xFF;
                    next_message->str = QString("%1 %2")
                                                    .arg(temp1)
                                                    .arg(temp2);
                    next_message->cmd = CMD_START_SESSION;
                    cmd_handler(next_message);
                    timer_repeat.start(LLMDB_TIMEOUT);
                    break;
                }
                default:
                    break;
                }
            }

            if (mode_session_download)
            {
                switch ((unsigned char)buf[0]) {
                case ((((CMD_FLASH_SESSION_SIZE >> 3) + CMD_FLASH_SESSION_SIZE) & 0x0F) | (0x03 << 4)):{
                    timer_repeat.stop();
                    memcpy(&session_download_block_number, &buf[1], 4*sizeof(unsigned char));
                    session_download_block_number /= 24;
                    next_message->cmd = CMD_FLASH_READ_SESSION_BLOCK;
                    session_download_block_counter = 1;
                    next_message->num = session_download_block_counter;
                    cmd_handler(next_message);
                    timer_repeat.start(LLMDB_TIMEOUT);
                    break;
                }
                case ((((CMD_FLASH_READ_SESSION_BLOCK >> 3) + CMD_FLASH_READ_SESSION_BLOCK) & 0x0F) | (0x03 << 4)):{
                    timer_repeat.stop();

                    session_download_block_counter++;
                    if (session_download_block_counter > session_download_block_number)
                    {
                        key = KEY_AUTO_MODE_OFF;
                        emit response(key, NULL, NULL, 0);
                        delete [] buf;
                        buf = NULL;
                        return;
                    }
                    next_message->num = session_download_block_counter;
                    cmd_handler(next_message);
                    timer_repeat.start(LLMDB_TIMEOUT);
                    break;
                }
                }
            }

            if (mode_flash_download)
            {
                switch ((unsigned char)buf[0]) {
                case (((CMD_FLASH_READ_BLOCK >> 3) + CMD_FLASH_READ_BLOCK) & 0x0F) | (0x03 << 4):
                    timer_repeat.stop();

                    key = KEY_PROGRESS_BAR;
                    emit response(key, NULL, buf, s);

                    flash_download_block_counter++;
                    if (flash_download_block_counter > flash_download_block_number)
                    {
                        key = KEY_AUTO_MODE_OFF;
                        emit response(key, NULL, NULL, 0);
                        delete [] buf;
                        buf = NULL;
                        return;
                    }
                    next_message->num = flash_download_block_counter;
                    cmd_handler(next_message);
                    timer_repeat.start(LLMDB_TIMEOUT);
                    break;
                default:
                    break;
                }
            }
        }

        delete [] buf;
        buf = NULL;
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

        sessions_rec_duration_random = true;
    }
    else // Manual
    {
        sessions_rec_duration = data[2];    // sec
        sessions_rec_duration_random = false;
    }

    // Set status request mode
    if (!data[3])
        timer_flash_status.start(data[4] * 1000);

    sessions_rec_contents_check_mode = data[5];
    sessions_rec_free_space_requset_mode = data[6];

    next_message->cmd = CMD_BINR_START;
    cmd_handler(next_message);
    timer_repeat.start(LLMDB_TIMEOUT);
}

void llmdb_client_ext::start_session_download(QString session_name_str)
{
    mode_none = false;
    mode_flash_download = false;
    mode_sessions_rec = false;
    mode_session_download = true;

    next_message->cmd = CMD_FLASH_SESSION_SIZE;
    next_message->str = session_name_str;
    cmd_handler(next_message);
    timer_repeat.start(LLMDB_TIMEOUT);
}

void llmdb_client_ext::start_flash_download(int *data)
{
    mode_none = false;
    mode_flash_download = true;
    mode_sessions_rec = false;
    mode_session_download = false;

    flash_download_block_counter = 1;

    if (!data[0])
        flash_download_block_number = data[1] * 22;
    else
        flash_download_block_number = FLASH_PAGES_QUANTITY * 22;

    next_message->cmd = CMD_FLASH_READ_BLOCK;
    next_message->num = flash_download_block_counter;
    cmd_handler(next_message);
    timer_repeat.start(LLMDB_TIMEOUT);
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
    timer_repeat.start(LLMDB_TIMEOUT);
}

void llmdb_client_ext::timer_flash_status_timeout()
{
    cmd_data* temp_message = new cmd_data;
    temp_message->dev_addr = next_message->dev_addr;
    temp_message->cmd = CMD_STATUS;
    cmd_handler(temp_message);
    delete temp_message;
    temp_message = NULL;
}

bool llmdb_client_ext::check_contents(char *data)
{
    bool ok = false;
    if (data[0] == 'M' &&
        data[1] == 'G' &&
        data[2] == 'P' &&
        data[3] == 'S'
            )
        ok = true;

    return ok;
}


