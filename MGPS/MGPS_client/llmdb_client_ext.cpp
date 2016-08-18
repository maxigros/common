#include "llmdb_client_ext.h"

llmdb_client_ext::llmdb_client_ext()
{
    mode_none = true;
    mode_flash_download = false;
    mode_sessions_rec = false;
    mode_session_download = false;
}

void llmdb_client_ext::initiate_mode(int new_mode)
{
    switch (new_mode) {
    case MODE_SESSIONS_REC:
        qDebug() << "sess rec";

        break;
    case MODE_SESSION_DOWNLOAD:
        qDebug() << "sess down";

        break;
    case MODE_FLASH_DOWNLOAD:
        qDebug() << "flash down";

        break;
    case MODE_NONE:
        qDebug() << "none";

        break;
    default:
        break;
    }
}
