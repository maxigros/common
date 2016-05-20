#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cp9_client = new cp9b_lib();

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
        ui->comboBox_COM->addItem(serialPortInfo.portName());

    connect(cp9_client, SIGNAL(send_to_gui(int,QString,char*,int)), this, SLOT(receive_from_cli(int,QString,char*,int)));
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

    cp9_client->send_to_COM(ar, msg_list.size());

    QString str;
    for(int a = 0; a < msg_list.size(); a++)
    {
        str += QString("%1 ").arg((unsigned char)ar[a], 2, 16, QChar('0'));
    }

    ui->text_log->append(">>>  OUT  >>>  : " + str);
}

void MainWindow::button_connect_clicked()
{
    cp9_client->connect_to_COM(ui->comboBox_COM->currentText());
}

void MainWindow::receive_from_cli(int key, QString msg, char *data, int data_size)
{
    switch (key)
    {
    case KEY_MSG_PACKSNUM:
        ui->statusBar->showMessage(QString("%1 packages sent, %2 packages received.")
                                        .arg(cp9_client->packs_sent)
                                        .arg(cp9_client->packs_received),
                                   0);
        cp9_client->packs_sent = 0;
        cp9_client->packs_received = 0;
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
    if (cp9_client->timer_test_mode)
    {
        cp9_client->timer_test_mode = false;
        ui->statusBar->showMessage(QString("%1 packages sent, %2 packages received.")
                                        .arg(cp9_client->packs_sent)
                                        .arg(cp9_client->packs_received),
                                   0);
        cp9_client->packs_sent = 0;
        cp9_client->packs_received = 0;
        cp9_client->tim.stop();
    }
    else
    {
        cp9_client->timer_test_mode = true;
        cp9_client->tim.start(ui->spinBox->value());
        cp9_client->packsnum = ui->spinBox_2->value();
        cp9_client->pack_size = ui->spinBox_3->value();
        ui->statusBar->clearMessage();
    }
}
