#ifndef LLMDB_CLIENT_EXT_H
#define LLMDB_CLIENT_EXT_H

#include "llmdb_client.h"
#include <QTimer>
#include <QFile>


class llmdb_client_ext : public llmdb_client
{
    Q_OBJECT
public:
    explicit llmdb_client_ext(QObject *parent = 0);
    void initiate_mode(int new_mode, unsigned char addr, int* data, QString str);

private:
    QTimer timer_repeat;
    QTimer timer_session_rec_duration;
    QTimer timer_flash_ask_free_space;
    cmd_data next_message;

    void onSocketDataReady();

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
     *      1 - duration mode (1 - interval, 0 - random)
     *      2 - duration (in sec)
     *      3 - status request mode (1 - interval, 0 - none)
     *      4 - status request interval
     *      5 - contents check (1 - need check, 0 - do not check)
     */
    void start_sessions_rec(int* data);
    int sessions_rec_counter;
    int sessions_rec_quantity;
    bool session_rec_in_progress = false;



    // Logs !!!

private slots:
    void timer_repeat_timeout();
    void timer_session_rec_duration_timeout();

};

#endif // LLMDB_CLIENT_EXT_H
