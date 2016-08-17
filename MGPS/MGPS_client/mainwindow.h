#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "defines.h"
#include "llmdb_client.h"

namespace Ui {
class MainWindow;
}

class llmdb_client;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void receive_data_from_client(QString msg);
    void openSock_trig();
    void closeSock_trig();
    void on_addr_edit_returnPressed();

private:
    Ui::MainWindow *ui;
    llmdb_client* client;

};

#endif // MAINWINDOW_H
