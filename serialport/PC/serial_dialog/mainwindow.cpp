#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    stuffing_client = new serial_stuffing();
    prefix_client = new serial_prefix();
    ui->groupBox_4->setEnabled(false);
    ui->text_log->append("Stuffing mode is on");

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
        ui->comboBox_COM->addItem(serialPortInfo.portName());

    connect(stuffing_client, SIGNAL(send_to_gui(int,QString,char*,int,int*,int)), this, SLOT(receive_from_cli(int,QString,char*,int,int*,int)));
    connect(prefix_client, SIGNAL(send_to_gui(int,QString,char*,int,int*,int)), this, SLOT(receive_from_cli(int,QString,char*,int,int*,int)));
    connect(ui->button_connect, SIGNAL(clicked(bool)), this, SLOT(button_connect_clicked()));
    connect(ui->button_send, SIGNAL(clicked(bool)), this, SLOT(button_send_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::button_send_clicked()
{
    QString message = ui->line_toSend->text().simplified();
    if (message.length() == 0)
        return;
    ui->line_toSend->clear();
    QStringList msg_list = message.split(" ");
    char ar[50];
    for (int i = 0; i < msg_list.size(); i++)
    {
        bool ok;
        int s = msg_list[i].toInt(&ok, 16);
        ar[i] = s;
    }

    if (stuffing_mode)
    {
        if (!stuffing_client->serial.isOpen())
        {
            ui->text_log->append("Serialport not open");
            return;
        }
        stuffing_client->send_to_COM(ar, msg_list.size());
    }

    if (prefix_mode)
    {
        if (!prefix_client->serial.isOpen())
        {
            ui->text_log->append("Serialport not open");
            return;
        }
        prefix_client->prefix_size = ui->spinBox_4->value();
        prefix_client->send_to_COM(ar, msg_list.size());
    }

    QString str;
    for(int a = 0; a < msg_list.size(); a++)
    {
        str += QString("%1 ").arg((unsigned char)ar[a], 2, 16, QChar('0'));
    }

//    ui->text_log->append(">>>  OUT  >>>  : " + str);
}

void MainWindow::button_connect_clicked()
{
    if (stuffing_mode)
    {
        if (prefix_client->serial.isOpen())
            prefix_client->serial.close();
        stuffing_client->connect_to_COM(ui->comboBox_COM->currentText());
    }

    if (prefix_mode)
    {
        if (stuffing_client->serial.isOpen())
            stuffing_client->serial.close();
        prefix_client->connect_to_COM(ui->comboBox_COM->currentText());
    }

}

void MainWindow::receive_from_cli(int key, QString msg, char *data, int data_size, int* nums, int nums_size)
{
    if (stuffing_mode)
        switch (key)
        {
        case KEY_MSG_PACKSNUM:
            ui->statusBar->showMessage(QString("%1 packages sent, %2 packages received.")
                                            .arg(stuffing_client->packs_sent)
                                            .arg(stuffing_client->packs_received),
                                       0);
            break;
        case KEY_MSG_PACKSNUM_END:
            ui->statusBar->showMessage(QString("%1 packages sent, %2 packages received.")
                                            .arg(nums[0])
                                            .arg(nums[1]),
                                       0);
            ui->text_log->append(msg);
            stuffing_client->packs_sent = 0;
            stuffing_client->packs_received = 0;
            break;
        case KEY_MSG_FROM_CLI:
            ui->text_log->append("### : " + msg);
            break;
        case KEY_MSG_FROM_DEV:
            QString str;
            for(int a=0; a<data_size; a++)
            {
                str += QString("%1 ").arg((unsigned char)data[a], 2, 16, QChar('0'));
            }
            ui->text_log->append("<<< IN <<< : " + str);
            break;
        }

    if (prefix_mode)
        switch (key)
        {
        case KEY_MSG_PACKSNUM:
            ui->statusBar->showMessage(QString("%1 packages sent, %2 packages received.")
                                            .arg(prefix_client->packs_sent)
                                            .arg(prefix_client->packs_received),
                                       0);
            break;
        case KEY_MSG_PACKSNUM_END:
            ui->statusBar->showMessage(QString("%1 packages sent, %2 packages received.")
                                            .arg(nums[0])
                                            .arg(nums[1]),
                                       0);
            ui->text_log->append(msg);
            prefix_client->packs_sent = 0;
            prefix_client->packs_received = 0;
            break;
        case KEY_MSG_FROM_CLI:
            ui->text_log->append("### : " + msg);
            break;
        case KEY_MSG_FROM_DEV:
            QString str;
            for(int a=0; a<data_size; a++)
            {
                str += QString("%1 ").arg((unsigned char)data[a], 2, 16, QChar('0'));
            }
            ui->text_log->append("<<< IN <<< : " + str);
            break;
        }

}

void MainWindow::on_button_timer_test_clicked()
{
    if (stuffing_mode)
    {
        if (!stuffing_client->timer_test_mode)
        {
            stuffing_client->timer_test_mode = true;
            stuffing_client->tim.start(ui->spinBox->value());
            stuffing_client->packsnum = ui->spinBox_2->value();
            stuffing_client->pack_size = ui->spinBox_3->value();
            ui->statusBar->clearMessage();
            ui->text_log->append(QString("Timetest on"));
        }
    }

    if (prefix_mode)
    {
        if (!prefix_client->timer_test_mode)
        {
            prefix_client->timer_test_mode = true;
            prefix_client->tim.start(ui->spinBox->value());
            prefix_client->packsnum = ui->spinBox_2->value();
            prefix_client->pack_size = ui->spinBox_3->value();
            prefix_client->prefix_size = ui->spinBox_4->value();
            ui->statusBar->clearMessage();
            ui->text_log->append(QString("Timetest on"));
        }
    }
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0: // stuffing
        ui->groupBox_4->setEnabled(false);
        stuffing_mode = true;
        prefix_mode = false;
        ui->text_log->append("Stuffing mode is on");
        break;
    case 1: // prefix
        ui->groupBox_4->setEnabled(true);
        stuffing_mode = false;
        prefix_mode = true;
        ui->text_log->append("Prefix mode is on");
        break;
    }
}

void MainWindow::on_pushButton_clicked()
{
    if (stuffing_mode)
    {
        if (stuffing_client->timer_test_mode)
        {
            stuffing_client->timer_test_mode = false;
            ui->statusBar->showMessage(QString("%1 packages sent, %2 packages received.")
                                            .arg(stuffing_client->packs_sent)
                                            .arg(stuffing_client->packs_received),
                                       0);
            ui->text_log->append(QString("Timetest off"));
            stuffing_client->packs_sent = 0;
            stuffing_client->packs_received = 0;
            stuffing_client->tim.stop();
        }
    }

    if (prefix_mode)
    {
        if (prefix_client->timer_test_mode)
        {
            prefix_client->timer_test_mode = false;
            ui->statusBar->showMessage(QString("%1 packages sent, %2 packages received.")
                                            .arg(prefix_client->packs_sent)
                                            .arg(prefix_client->packs_received),
                                       0);
            ui->text_log->append(QString("Timetest off"));
            prefix_client->packs_sent = 0;
            prefix_client->packs_received = 0;
            prefix_client->tim.stop();
        }
    }
}
