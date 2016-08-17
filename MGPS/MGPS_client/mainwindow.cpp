#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    client = new llmdb_client();
    client->host_addr = ui->addr_edit->text();
    connect(client, SIGNAL(response(QString)), this, SLOT(receive_data_from_client(QString)));
    connect(ui->openSock_action, SIGNAL(triggered(bool)), this, SLOT(openSock_trig()));
    connect(ui->closeSock_action, SIGNAL(triggered(bool)), this, SLOT(closeSock_trig()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openSock_trig()
{
    client->open_socket();
}

void MainWindow::closeSock_trig()
{
    client->close_socket();
}

void MainWindow::receive_data_from_client(QString msg)
{
    qDebug() << msg;
}


void MainWindow::on_addr_edit_returnPressed()
{
    client->host_addr = ui->addr_edit->text();
}
