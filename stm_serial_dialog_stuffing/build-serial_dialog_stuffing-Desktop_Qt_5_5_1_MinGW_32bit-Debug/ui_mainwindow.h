/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QComboBox *comboBox_COM;
    QPushButton *button_connect;
    QSpacerItem *horizontalSpacer_2;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer;
    QPushButton *button_timer_test;
    QCheckBox *check_echo_mode;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *line_toSend;
    QPushButton *button_send;
    QVBoxLayout *verticalLayout;
    QTextEdit *text_log;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *pushButton_3;
    QSpacerItem *horizontalSpacer_4;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1067, 630);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout_3 = new QVBoxLayout(centralWidget);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        comboBox_COM = new QComboBox(groupBox);
        comboBox_COM->setObjectName(QStringLiteral("comboBox_COM"));

        horizontalLayout->addWidget(comboBox_COM);

        button_connect = new QPushButton(groupBox);
        button_connect->setObjectName(QStringLiteral("button_connect"));

        horizontalLayout->addWidget(button_connect);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);


        horizontalLayout_2->addWidget(groupBox);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout_3->addLayout(horizontalLayout_2);

        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        verticalLayout_2 = new QVBoxLayout(groupBox_2);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);

        button_timer_test = new QPushButton(groupBox_2);
        button_timer_test->setObjectName(QStringLiteral("button_timer_test"));

        horizontalLayout_5->addWidget(button_timer_test);

        check_echo_mode = new QCheckBox(groupBox_2);
        check_echo_mode->setObjectName(QStringLiteral("check_echo_mode"));

        horizontalLayout_5->addWidget(check_echo_mode);


        verticalLayout_2->addLayout(horizontalLayout_5);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        line_toSend = new QLineEdit(groupBox_2);
        line_toSend->setObjectName(QStringLiteral("line_toSend"));

        horizontalLayout_3->addWidget(line_toSend);

        button_send = new QPushButton(groupBox_2);
        button_send->setObjectName(QStringLiteral("button_send"));

        horizontalLayout_3->addWidget(button_send);


        verticalLayout_2->addLayout(horizontalLayout_3);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        text_log = new QTextEdit(groupBox_2);
        text_log->setObjectName(QStringLiteral("text_log"));

        verticalLayout->addWidget(text_log);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);

        pushButton_3 = new QPushButton(groupBox_2);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));

        horizontalLayout_4->addWidget(pushButton_3);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout_4);


        verticalLayout_2->addLayout(verticalLayout);


        verticalLayout_3->addWidget(groupBox_2);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1067, 21));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);
        QObject::connect(pushButton_3, SIGNAL(clicked()), text_log, SLOT(clear()));
        QObject::connect(line_toSend, SIGNAL(returnPressed()), button_send, SLOT(click()));
        QObject::connect(pushButton_3, SIGNAL(clicked()), statusBar, SLOT(clearMessage()));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "serial_stuffing", 0));
        groupBox->setTitle(QApplication::translate("MainWindow", "COM port :", 0));
        button_connect->setText(QApplication::translate("MainWindow", "Connect", 0));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Dialog", 0));
        button_timer_test->setText(QApplication::translate("MainWindow", "Timer test", 0));
        check_echo_mode->setText(QApplication::translate("MainWindow", "stuffing", 0));
        button_send->setText(QApplication::translate("MainWindow", "Send", 0));
        pushButton_3->setText(QApplication::translate("MainWindow", "Clear", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
