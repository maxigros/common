#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "defines.h"
#include "llmdb_client_ext.h"

namespace Ui {
class MainWindow;
}

class llmdb_client_ext;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void receive_data_from_client(int key, QString msg, char* data, int size);

    void mode_handler(int new_mode);

    void on_host_addr_line_returnPressed();

    void on_button_open_socket_clicked();

    void on_button_close_socket_clicked();

    void on_button_session_stop_clicked();

    void on_button_binr_start_clicked();

    void on_button_binr_stop_clicked();

    void on_button_flash_erase_clicked();

    void on_button_flash_fat_clicked();

    void on_button_flash_free_space_clicked();

    void on_button_flash_first_block_clicked();

    void on_button_session_start_clicked();

    void on_button_flash_read_block_clicked();

    void on_button_session_size_clicked();

    void on_button_session_block_clicked();

    void on_button_device_status_clicked();

    void on_button_sessions_rec_start_clicked();

    void on_button_session_size_refresh_clicked();

    void on_button_session_download_start_clicked();

    void on_button_flash_download_start_clicked();

    void on_pushButton_auto_stop_all_clicked();

    void on_lineEdit_send_returnPressed();

    void on_comboBox_session_rec_duration_mode_currentIndexChanged(int index);

    void on_comboBox_session_rec_status_mode_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    llmdb_client_ext* client;
    mode_task* task;

signals:
    void mode_changed(int new_mode);

};

#endif // MAINWINDOW_H
