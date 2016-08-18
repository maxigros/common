#ifndef LLMDB_CLIENT_EXT_H
#define LLMDB_CLIENT_EXT_H

#include "llmdb_client.h"
#include <QTimer>
#include <QFile>


class llmdb_client_ext : public llmdb_client
{
public:
    llmdb_client_ext();
    void initiate_mode(int new_mode);

private:
    QTimer timer_repeat;
    QTimer timer_flash_ask_free_space;

    // Modes:
    bool mode_none;
    bool mode_sessions_rec;
    bool mode_session_download;
    bool mode_flash_download;

    // Logs !!!

};

#endif // LLMDB_CLIENT_EXT_H
