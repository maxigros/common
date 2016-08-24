#ifndef LLMDB_CLIENT_EXT_H
#define LLMDB_CLIENT_EXT_H

#include "llmdb_client.h"
#include <QTimer>


class llmdb_client_ext : public llmdb_client
{
    Q_OBJECT
public:
    explicit llmdb_client_ext(QObject *parent = 0);
    void initiate_mode(mode_task* task);

private:
    QTimer timer_repeat;
    QTimer timer_session_rec_duration;
    QTimer timer_flash_status;
    cmd_data* next_message;

    void onSocketDataReady();
    void timers_stop();

    // Modes:
    bool mode_none;
    bool mode_sessions_rec;
    bool mode_session_download;
    bool mode_flash_download;

    /*
     * SESSIONS REC mode
     * Rotation:
     *
     * CMD_BINR_START
     * CMD_START_SESSION
     * CMD_STOP_SESSION
     * CMD_START_SESSION
     * CMD_STOP_SESSION
     * ................
     *
     * Note: CMD_STATUS is being requested every 1 sec
     *
     * Arguments:
     * data - 6 integers:
     *      0 - sessions quantity
     *      1 - duration mode (0 - interval, 1 - random)
     *      2 - duration (in sec)
     *      3 - status request mode (0 - interval, 1 - none)
     *      4 - status request interval
     *      5 - contents check (1 - need check, 0 - do not check)
     */
    void start_sessions_rec(int* data);
    int sessions_rec_counter;
    int sessions_rec_quantity;
    int sessions_rec_duration;
    bool sessions_rec_duration_random;
    int sessions_rec_contents_check_mode;
    bool session_rec_in_progress = false;
    bool check_contents(char *data);

    /*
     *  SESSION DOWNLOAD mode
     */
    void start_session_download(QString session_name_str);
    int session_download_block_counter;
    int session_download_block_number;

    /*
     *  FLASH DOWNLOAD mode
     *
     *  Arguments:
     * data - 2 integers:
     *      0 - mode (0 - specified number of pages, 1 - full flash)
     *      1 - number of pages, set by user
     */
    void start_flash_download(int *data);
    int flash_download_block_counter;
    int flash_download_block_number;


private slots:
    void timer_repeat_timeout();
    void timer_session_rec_duration_timeout();
    void timer_flash_status_timeout();

};

#endif // LLMDB_CLIENT_EXT_H
