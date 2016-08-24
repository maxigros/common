#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    client = new llmdb_client_ext();
    client->host_addr = ui->host_addr_line->text();
    connect(client, SIGNAL(response(int, QString, char*, int)), this, SLOT(receive_data_from_client(int, QString, char*, int)));
    connect(this, SIGNAL(mode_changed(int)), this, SLOT(mode_handler(int)));

}

MainWindow::~MainWindow()
{
    delete client;
    delete ui;
    if (general_log.isOpen())
        general_log.close();
}


void MainWindow::receive_data_from_client(int key, QString msg, char *data, int size)
{
    size = size; // warning 'unused variable'
    ui->textEdit_log->append(msg);
    if (ui->checkBox_general_log->isChecked())
    {
        general_log_stream << msg << endl;
        general_log.flush();
    }

    if (!key)
        return;

    switch (key) {
    case KEY_STATUS:{
        ui->label_status_1->setText(QString::number((int)(data[1] & 0b00000001) >> 0));
        ui->label_status_2->setText(QString::number((int)(data[1] & 0b00000010) >> 1));
        ui->label_status_3->setText(QString::number((int)(data[1] & 0b00000100) >> 2));
        ui->label_status_4->setText(QString::number((int)(data[1] & 0b00001000) >> 3));
        ui->label_status_5->setText(QString::number((int)(data[1] & 0b00010000) >> 4));
        ui->label_status_6->setText(QString::number((int)(data[1] & 0b00100000) >> 5));
        ui->label_status_7->setText(QString::number((int)(data[1] & 0b01000000) >> 6));
        break;
    }
    case KEY_SESSION_SIZE:{
        int *t = new int;
        memcpy(t, data + sizeof(unsigned char), 4*sizeof(unsigned char));
        ui->label_session_size_bytes->setText(QString("%1 bytes").arg(*t));
        ui->label_session_size_blocks->setText(QString("%1 blocks").arg(*t / 24));
        ui->label_session_size_pages->setText(QString("%1 pages").arg(*t / 24 / 22));

        ui->label_session_download_size->setText(QString("%1 blocks").arg(*t / 24));
        delete t;
        break;
    }
    case KEY_SESSION_SIZE_ERROR:{
        ui->label_session_size_bytes->setText("error");
        ui->label_session_download_size->setText("error");
        if (session_download_log.isOpen())
            session_download_log.close();
        break;
    }
    case KEY_FLASH_FREE_SPACE:{
        int *t = new int;
        memcpy(t, data + sizeof(unsigned char), 4*sizeof(unsigned char));
        ui->label_flash_free_space_bytes->setText(QString("%1 bytes").arg(*t));
        ui->label_flash_free_space_pages->setText(QString("%1 pages").arg(*t / 528));
        delete t;
        break;
    }
    case KEY_AUTO_MODE_OFF:{
        emit mode_changed(MODE_NONE);
        break;
    }
    case KEY_FLASH_CONTENTS_ERROR:{
        ui->statusBar->showMessage("Contents Error");
        break;
    }
    case KEY_SESSION_DOWNLOAD_LOG:{
        QByteArray *temp_array = new QByteArray(data, size);
        temp_array->remove(0, 5);
        temp_array->remove(temp_array->size() - 1, 1);
        session_download_log.write(*temp_array);
        delete temp_array;
        break;
    }
    case KEY_FLASH_DOWNLOAD_LOG:{
        QByteArray *temp_array = new QByteArray(data, size);
        temp_array->remove(0, 5);
        temp_array->remove(temp_array->size() - 1, 1);
        flash_download_log.write(*temp_array);
        delete temp_array;
        break;
    }
    case KEY_PROGRESS_BAR:{
        int* temp_pages = new int;
        memcpy(temp_pages, data + sizeof(unsigned char), 4*sizeof(unsigned char));
        *temp_pages /= 22;

        int* all_blocks = new int;
        if (ui->comboBox_flash_download->currentIndex())
            *all_blocks = FLASH_PAGE_NUMBER;
        else
            *all_blocks = ui->lineEdit_flash_download_blocks_num->text().toInt();

        double* temp_div = new double;
        *temp_div = (double)*temp_pages / (double)*all_blocks;

        ui->progressBar_flash_download->setValue(*temp_div * 100.0);

        delete temp_pages;
        delete all_blocks;
        delete temp_div;
        temp_pages = NULL;
        all_blocks = NULL;
        temp_div = NULL;
        break;
    }
    default:
        break;
    }
}

void MainWindow::mode_handler(int new_mode)
{
    ui->statusBar->clearMessage();
    switch (new_mode) {
    case MODE_SESSIONS_REC:{
        ui->groupBox_sessions_rec->setEnabled(false);
        ui->groupBox_session_download->setEnabled(false);
        ui->groupBox_flash_download->setEnabled(false);
        ui->statusBar->showMessage("Sessions Rec Active!", 0);

        task = new mode_task;
        task->mode = MODE_SESSIONS_REC;
        task->dev_addr = ui->device_addr_line->text().toInt();
        // Sessions quantity
        task->data[0] = ui->lineEdit_session_rec_quantity->text().toInt();
        // Duration mode
        task->data[1] = ui->comboBox_session_rec_duration_mode->currentIndex();
        // Duration (sec)
        task->data[2] = ui->lineEdit_session_rec_duration->text().toInt();
        // Status request mode
        task->data[3] = ui->comboBox_session_rec_status_mode->currentIndex();
        // Status request interval
        task->data[4] = ui->lineEdit_session_rec_status->text().toInt();
        // Contents check
        if (ui->checkBox_sessions_rec_contents->isChecked())
            task->data[5] = 1;
        else
            task->data[5] = 0;

        client->initiate_mode(task);
        delete task;
        break;
        }
    case MODE_SESSION_DOWNLOAD:{
        ui->groupBox_sessions_rec->setEnabled(false);
        ui->groupBox_session_download->setEnabled(false);
        ui->groupBox_flash_download->setEnabled(false);
        ui->statusBar->showMessage("Session Download Active!", 0);

        task = new mode_task;
        task->mode = MODE_SESSION_DOWNLOAD;
        task->dev_addr = ui->device_addr_line->text().toInt();
        task->str = ui->lineEdit_session_download_name->text();

        if (!QDir("logs").exists())
            QDir().mkdir("logs");
        session_download_log.setFileName(QString("logs/session(%1)_log(%2).log")
                                          .arg(task->str)
                                          .arg(QDateTime::currentDateTime().toString("dd.MM_hh:mm:ss")));
        session_download_log.open(QFile::WriteOnly);

        client->initiate_mode(task);
        delete task;
        task = NULL;
        break;
    }
    case MODE_FLASH_DOWNLOAD:{
        ui->groupBox_sessions_rec->setEnabled(false);
        ui->groupBox_session_download->setEnabled(false);
        ui->groupBox_flash_download->setEnabled(false);
        ui->statusBar->showMessage("Flash Download Active!", 0);

        task = new mode_task;
        task->mode = MODE_FLASH_DOWNLOAD;
        task->dev_addr = ui->device_addr_line->text().toInt();
        task->data[0] = ui->comboBox_flash_download->currentIndex();
        task->data[1] = ui->lineEdit_flash_download_blocks_num->text().toInt();

        if (!QDir("logs").exists())
            QDir().mkdir("logs");
        if (!task->data[0])
        {
            flash_download_log.setFileName(QString("logs/flash_first(%1)_log(%2).log")
                                              .arg(task->data[1])
                                              .arg(QDateTime::currentDateTime().toString("dd.MM_hh:mm:ss")));
        }
        else
        {
            flash_download_log.setFileName(QString("logs/flash_full_log(%1).log")
                                              .arg(QDateTime::currentDateTime().toString("dd.MM_hh:mm:ss")));
        }
        flash_download_log.open(QFile::WriteOnly);

        client->initiate_mode(task);
        delete task;
        task = NULL;
        break;
    }
    case MODE_NONE:{
        ui->groupBox_sessions_rec->setEnabled(true);
        ui->groupBox_session_download->setEnabled(true);
        ui->groupBox_flash_download->setEnabled(true);
        ui->statusBar->clearMessage();

        if (session_download_log.isOpen())
            session_download_log.close();

        if (flash_download_log.isOpen())
            flash_download_log.close();

        task = new mode_task;
        task->mode = MODE_NONE;
        client->initiate_mode(task);
        delete task;
        break;
    }
    default:
        break;
    }
}


void MainWindow::on_host_addr_line_returnPressed()
{
    client->host_addr = ui->host_addr_line->text();
}

void MainWindow::on_button_open_socket_clicked()
{
    client->open_socket();
}

void MainWindow::on_button_close_socket_clicked()
{
    client->close_socket();
}

void MainWindow::on_button_session_stop_clicked()
{
    cmd_data* command = new cmd_data;
    command->dev_addr = ui->device_addr_line->text().toInt();
    command->cmd = CMD_STOP_SESSION;
    client->cmd_handler(command);
    delete command;
    command = NULL;
}

void MainWindow::on_button_binr_start_clicked()
{
    cmd_data* command = new cmd_data;
    command->dev_addr = ui->device_addr_line->text().toInt();
    command->cmd = CMD_BINR_START;
    client->cmd_handler(command);
    delete command;
    command = NULL;
}

void MainWindow::on_button_binr_stop_clicked()
{
    cmd_data* command = new cmd_data;
    command->dev_addr = ui->device_addr_line->text().toInt();
    command->cmd = CMD_BINR_STOP;
    client->cmd_handler(command);
    delete command;
    command = NULL;
}

void MainWindow::on_button_flash_erase_clicked()
{
    cmd_data* command = new cmd_data;
    command->dev_addr = ui->device_addr_line->text().toInt();
    command->cmd = CMD_FLASH_ERASE;
    client->cmd_handler(command);
    delete command;
    command = NULL;
}

void MainWindow::on_button_flash_fat_clicked()
{
    cmd_data* command = new cmd_data;
    command->dev_addr = ui->device_addr_line->text().toInt();
    command->cmd = CMD_FLASH_MAKE_FAT;
    client->cmd_handler(command);
    delete command;
    command = NULL;
}

void MainWindow::on_button_flash_free_space_clicked()
{
    cmd_data* command = new cmd_data;
    command->dev_addr = ui->device_addr_line->text().toInt();
    command->cmd = CMD_FLASH_FREE_SPACE;
    client->cmd_handler(command);
    delete command;
    command = NULL;
}

void MainWindow::on_button_flash_first_block_clicked()
{
    cmd_data* command = new cmd_data;
    command->dev_addr = ui->device_addr_line->text().toInt();
    command->cmd = CMD_FLASH_READ_BLOCK;
    command->num = 1;
    client->cmd_handler(command);
    delete command;
    command = NULL;
}

void MainWindow::on_button_session_start_clicked()
{
    cmd_data* command = new cmd_data;
    command->dev_addr = ui->device_addr_line->text().toInt();
    command->cmd = CMD_START_SESSION;
    command->str = ui->lineEdit_session_start_name->text();
    client->cmd_handler(command);
    delete command;
    command = NULL;
}

void MainWindow::on_button_flash_read_block_clicked()
{
    cmd_data* command = new cmd_data;
    command->dev_addr = ui->device_addr_line->text().toInt();
    command->cmd = CMD_FLASH_READ_BLOCK;
    command->num = ui->lineEdit_read_block_number->text().toInt();
    client->cmd_handler(command);
    delete command;
    command = NULL;
}

void MainWindow::on_button_session_size_clicked()
{
    cmd_data* command = new cmd_data;
    command->dev_addr = ui->device_addr_line->text().toInt();
    command->cmd = CMD_FLASH_SESSION_SIZE;
    command->str = ui->lineEdit_session_size_name->text();
    client->cmd_handler(command);
    delete command;
    command = NULL;
}

void MainWindow::on_button_session_block_clicked()
{
    cmd_data* command = new cmd_data;
    command->dev_addr = ui->device_addr_line->text().toInt();
    command->cmd = CMD_FLASH_READ_SESSION_BLOCK;
    command->str = ui->lineEdit_session_block_name->text();
    command->num = ui->lineEdit_session_block_number->text().toInt();
    client->cmd_handler(command);
    delete command;
    command = NULL;
}

void MainWindow::on_button_device_status_clicked()
{
    cmd_data* command = new cmd_data;
    command->dev_addr = ui->device_addr_line->text().toInt();
    command->cmd = CMD_STATUS;
    client->cmd_handler(command);
    delete command;
    command = NULL;
}

void MainWindow::on_button_sessions_rec_start_clicked()
{
    emit mode_changed(MODE_SESSIONS_REC);
}

void MainWindow::on_button_session_download_check_clicked()
{
    cmd_data* command = new cmd_data;
    command->dev_addr = ui->device_addr_line->text().toInt();
    command->cmd = CMD_FLASH_SESSION_SIZE;
    command->str = ui->lineEdit_session_download_name->text();
    client->cmd_handler(command);
    delete command;
    command = NULL;
}

void MainWindow::on_button_session_download_start_clicked()
{
    emit mode_changed(MODE_SESSION_DOWNLOAD);
}

void MainWindow::on_button_flash_download_start_clicked()
{
    emit mode_changed(MODE_FLASH_DOWNLOAD);
}

void MainWindow::on_pushButton_auto_stop_all_clicked()
{
    emit mode_changed(MODE_NONE);
}

void MainWindow::on_lineEdit_send_returnPressed()
{
    cmd_data* command = new cmd_data;
    command->dev_addr = ui->device_addr_line->text().toInt();
    command->cmd = SERVICE_CMD_MANUAL_DATA;
    command->str = ui->lineEdit_send->text();
    ui->lineEdit_send->clear();
    client->cmd_handler(command);
    delete command;
    command = NULL;
}

void MainWindow::on_comboBox_session_rec_duration_mode_currentIndexChanged(int index)
{
    if (index)
        ui->lineEdit_session_rec_duration->setEnabled(false);
    else
        ui->lineEdit_session_rec_duration->setEnabled(true);
}

void MainWindow::on_comboBox_session_rec_status_mode_currentIndexChanged(int index)
{
    if (index)
        ui->lineEdit_session_rec_status->setEnabled(false);
    else
        ui->lineEdit_session_rec_status->setEnabled(true);
}

void MainWindow::on_checkBox_general_log_stateChanged(int arg1)
{
    if (arg1)
    {
        if (!QDir("logs").exists())
            QDir().mkdir("logs");
        general_log.setFileName(QString("logs/general_log(%1).txt").arg(QDateTime::currentDateTime().toString("dd.MM_hh:mm:ss")));
        general_log.open(QFile::WriteOnly | QFile::Text);
        general_log_stream.setDevice(&general_log);
    }
    else
    {
        general_log.close();
    }
}

void MainWindow::on_comboBox_flash_download_currentIndexChanged(int index)
{
    if (!index)
        ui->lineEdit_flash_download_blocks_num->setEnabled(true);
    else
        ui->lineEdit_flash_download_blocks_num->setEnabled(false);
}
