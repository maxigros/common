#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    client = new llmdb_client();
    client->host_addr = ui->host_addr_line->text();
    connect(client, SIGNAL(response(int, QString, char*, int)), this, SLOT(receive_data_from_client(int, QString, char*, int)));
}

MainWindow::~MainWindow()
{
    delete client;
    delete ui;
}


void MainWindow::receive_data_from_client(int key, QString msg, char *data, int size)
{
    ui->textEdit_log->append(msg);
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
        int t;
        memcpy(&t, data + sizeof(unsigned char), 4*sizeof(unsigned char));
        ui->label_session_size_bytes->setText(QString("%1 bytes").arg(t));
        ui->label_session_size_blocks->setText(QString("%1 blocks").arg(t / 24));
        ui->label_session_size_pages->setText(QString("%1 pages").arg(t / 24 / 22));
        break;
    }
    case KEY_FLASH_FREE_SPACE:{
        int t;
        memcpy(&t, data + sizeof(unsigned char), 4*sizeof(unsigned char));
        ui->label_flash_free_space_bytes->setText(QString("%1 bytes").arg(t));
        ui->label_flash_free_space_pages->setText(QString("%1 pages").arg(t / 528));
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
    cmd_data command;
    command.dev_addr = ui->device_addr_line->text().toInt();
    command.cmd = CMD_STOP_SESSION;
    client->cmd_handler(command);
}

void MainWindow::on_button_binr_start_clicked()
{
    cmd_data command;
    command.dev_addr = ui->device_addr_line->text().toInt();
    command.cmd = CMD_BINR_START;
    client->cmd_handler(command);
}

void MainWindow::on_button_binr_stop_clicked()
{
    cmd_data command;
    command.dev_addr = ui->device_addr_line->text().toInt();
    command.cmd = CMD_BINR_STOP;
    client->cmd_handler(command);
}

void MainWindow::on_button_flash_erase_clicked()
{
    cmd_data command;
    command.dev_addr = ui->device_addr_line->text().toInt();
    command.cmd = CMD_FLASH_ERASE;
    client->cmd_handler(command);
}

void MainWindow::on_button_flash_fat_clicked()
{
    cmd_data command;
    command.dev_addr = ui->device_addr_line->text().toInt();
    command.cmd = CMD_FLASH_MAKE_FAT;
    client->cmd_handler(command);
}

void MainWindow::on_button_flash_free_space_clicked()
{
    cmd_data command;
    command.dev_addr = ui->device_addr_line->text().toInt();
    command.cmd = CMD_FLASH_FREE_SPACE;
    client->cmd_handler(command);
}

void MainWindow::on_button_flash_first_block_clicked()
{
    cmd_data command;
    command.dev_addr = ui->device_addr_line->text().toInt();
    command.cmd = CMD_FLASH_READ_BLOCK;
    command.num = 1;
    client->cmd_handler(command);
}

void MainWindow::on_button_session_start_clicked()
{
    cmd_data command;
    command.dev_addr = ui->device_addr_line->text().toInt();
    command.cmd = CMD_START_SESSION;
    command.str = ui->lineEdit_session_start_name->text();
    client->cmd_handler(command);
}

void MainWindow::on_button_flash_read_block_clicked()
{
    cmd_data command;
    command.dev_addr = ui->device_addr_line->text().toInt();
    command.cmd = CMD_FLASH_READ_BLOCK;
    command.num = ui->lineEdit_read_block_number->text().toInt();
    client->cmd_handler(command);
}

void MainWindow::on_button_session_size_clicked()
{
    cmd_data command;
    command.dev_addr = ui->device_addr_line->text().toInt();
    command.cmd = CMD_FLASH_SESSION_SIZE;
    command.str = ui->lineEdit_session_size_name->text();
    client->cmd_handler(command);
}

void MainWindow::on_button_session_block_clicked()
{
    cmd_data command;
    command.dev_addr = ui->device_addr_line->text().toInt();
    command.cmd = CMD_FLASH_READ_SESSION_BLOCK;
    command.str = ui->lineEdit_session_block_name->text();
    command.num = ui->lineEdit_session_block_number->text().toInt();
    client->cmd_handler(command);
}

void MainWindow::on_button_device_status_clicked()
{
    cmd_data command;
    command.dev_addr = ui->device_addr_line->text().toInt();
    command.cmd = CMD_STATUS;
    client->cmd_handler(command);
}

void MainWindow::on_button_session_rec_start_clicked()
{

}

void MainWindow::on_button_session_size_refresh_clicked()
{

}

void MainWindow::on_button_session_download_start_clicked()
{

}

void MainWindow::on_button_flash_download_start_clicked()
{

}

void MainWindow::on_checkBox_contents_toggled(bool checked)
{

}

void MainWindow::on_lineEdit_send_returnPressed()
{
    cmd_data command;
    command.dev_addr = ui->device_addr_line->text().toInt();
    command.cmd = SERVICE_CMD_MANUAL_DATA;
    command.str = ui->lineEdit_send->text();
    ui->lineEdit_send->clear();
    client->cmd_handler(command);
}
